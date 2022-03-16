#include "auto.h"
#include "pros/apix.h"
#include "display/lvgl.h"
#define LOG_LEVEL_FILE LOG_LEVEL_DEBUG
#include "log.h"


/* System global variables */
static int active_mode;
static auto_pos_t active_pos;
static auto_color_t active_color;
static const auto_routine_t * local_auto_list;
static size_t local_auto_length;
static int active_auto;

/* Mapping of active mode to active pos */
static const auto_pos_t active_pos_mode[] = 
{
	AUTO_POS_1,
	AUTO_POS_2,
	AUTO_POS_1,
	AUTO_POS_2,
	AUTO_POS_SKILLS
};

/* Mapping of active mode to active color */
static const auto_pos_t active_color_mode[] = 
{
	AUTO_COLOR_RED,
	AUTO_COLOR_RED,
	AUTO_COLOR_BLUE,
	AUTO_COLOR_BLUE,
	AUTO_COLOR_SKILLS
};

/* Number of buttons we can fit on the side of the screen at once - will rescale
 * If you have more than this many autos per position, the rest won't show up
 */
#define LIST_SIZE 5

/* List box to select program */
static lv_obj_t * list_btn[LIST_SIZE];

/* 5 buttons for starting positions (4 match, 1 skills) */
static lv_obj_t * btn_pos[5];
static char * btn_label[5] = 
{
	"Red 1",
	"Red 2",
	"Blue 1",
	"Blue 2",
	"Skills"
};
static lv_align_t btn_align[5] = 
{
	LV_ALIGN_IN_TOP_LEFT,
	LV_ALIGN_IN_BOTTOM_LEFT,
	LV_ALIGN_IN_BOTTOM_RIGHT,
	LV_ALIGN_IN_TOP_RIGHT,
	LV_ALIGN_CENTER
};

/* Styles */
static lv_style_t style_red_ina, style_red_act;
static lv_style_t style_blue_ina, style_blue_act;
static lv_style_t style_gold_ina, style_gold_act;
static lv_style_t style_white_ina, style_white_act;

/* Styles associated with each starting position button in the inactive and active states */
static lv_style_t * btn_styles[5][2] = 
{
	{&style_red_ina, &style_red_act},
	{&style_red_ina, &style_red_act},
	{&style_blue_ina, &style_blue_act},
	{&style_blue_ina, &style_blue_act},
	{&style_gold_ina, &style_gold_act}
};


/* List callback */
static  lv_res_t btn_list_cb(lv_obj_t * btn)
{
	LOG_DEBUG("Got list callback");
	int idx;
	for(idx = 0; idx < LIST_SIZE; idx++)
	{
		if(btn == list_btn[idx])
		{
			break;
		}
	}
	LOG_DEBUG("List object %d",idx);

	/* Toggle off all buttons but toggle on ours */
	for(int i = 0; i < LIST_SIZE; i++)
	{
		/* Ignore buttons which are null */
		if((list_btn[i]) && (i == idx))
		{
			/* Toggle on */
			LOG_DEBUG("Turning ON button %d",i);
			lv_obj_set_style(list_btn[i],&style_white_act);
		}
		else if(list_btn[i])
		{
			/* Toggle off */
			LOG_DEBUG("Turning OFF button %d",i);
			lv_obj_set_style(list_btn[i],&style_white_ina);
		}
	}

	/* Determine active program to run:
	 * Go through the entire auto list to find ones matching the active pos
	 * Find the nth one in the list (matches idx)
	 * Find the i of that, as the active program 
	 */
	int valid_count = 0;
	for(int i = 0; i < local_auto_length; i++)
	{
		/* Does the position match? */
		if(local_auto_list[i].position == active_pos)
		{
			/* Check if active count matches idx */
			if(valid_count == idx)
			{
				/* Found what we are looking for, set it and break */
				LOG_INFO("Found correct autonomous: %s",local_auto_list[i].name);
				active_auto = i;
				break;
			}
			/* Otherwise, increment valid_count */
			valid_count++;
		}
	}
	
}
/* Position Button callback */
static  lv_res_t btn_pos_cb(lv_obj_t * btn)
{
    /* Find which index we are */
	int idx;
	for(idx = 0; idx < 5; idx++)
	{
		if(btn == btn_pos[idx])
		{
			break;
		}
	}
	active_mode = idx;
	/* Toggle off all buttons but toggle on ours */
	for(int i = 0; i < 5; i++)
	{
		if(i == idx)
		{
			/* Toggle on */
			lv_obj_set_style(btn_pos[i],btn_styles[i][1]);
		}
		else
		{
			/* Toggle off */
			lv_obj_set_style(btn_pos[i],btn_styles[i][0]);
		}
	}
	
	/* If the valid auto list was previously in existence, delete the buttons */
	for(int i = 0; i < LIST_SIZE; i++)
	{
		/* If object exists, delete it */
		if(list_btn[i])
		{
			lv_obj_del(list_btn[i]);
		}
		/* Clean up reference */
		list_btn[i] = NULL;
	}

	/* Convert active mode into a starting position and color */
	active_pos = active_pos_mode[active_mode];
	active_color = active_color_mode[active_mode];

	/* Determine how many valid routines there are for this starting position
	 * Also get an array of the pointers to their names
	 */
	int valid_autos = 0;
	char * valid_auto_names[LIST_SIZE] = {0};
	for(int i = 0; i < local_auto_length; i++)
	{
		LOG_DEBUG("Checking auto %s (pos %d)",local_auto_list[i].name,local_auto_list[i].position);
		if(local_auto_list[i].position == active_pos)
		{
			LOG_INFO("Auto %s is valid for the selected position",local_auto_list[i].name);
			valid_auto_names[valid_autos] = local_auto_list[i].name;
			valid_autos++;
		}
	}

	LOG_DEBUG("Got %d valid autos for this position",valid_autos);

	/* Check if we have too many autos */
	if(valid_autos > LIST_SIZE)
	{
		LOG_WARN("Too many auto modes to fit!");
		valid_autos = LIST_SIZE;
	}
	/* Determine how bit the buttons should be */
	int bwidth = lv_obj_get_width(lv_scr_act()) - 240;
	int bheight = lv_obj_get_height(lv_scr_act()) / LIST_SIZE;
	LOG_DEBUG("List buttons will be %d by %d",bwidth,bheight);
	/* Create buttons for this list */
	for(int i = 0; i < valid_autos; i++)
	{
		/* Create button */
		list_btn[i] = lv_btn_create(lv_scr_act(), NULL);
		lv_btn_set_action(list_btn[i],LV_BTN_ACTION_CLICK,btn_list_cb);

		/* Set Style */
		lv_btn_set_style(list_btn[i],LV_BTN_STYLE_INA,&style_white_ina);
		lv_btn_set_style(list_btn[i],LV_BTN_STYLE_PR,&style_white_act);
		lv_btn_set_style(list_btn[i],LV_BTN_STYLE_REL,&style_white_ina);

		/* Size the button */
		lv_obj_set_size(list_btn[i], bwidth, bheight);

		/* Align button to top right offset by button number */
		lv_obj_align(list_btn[i],lv_scr_act(),LV_ALIGN_IN_TOP_RIGHT,0,i*bheight);

		/* Add label */
		lv_obj_t * label = lv_label_create(list_btn[i],NULL);
		lv_label_set_text(label,valid_auto_names[i]);
	}

    return LV_RES_OK;
}


void auto_picker(const auto_routine_t * list, size_t length)
{
    LOG_ALWAYS("Auto Picker running");
    /* Save arguments to be used later */
    local_auto_list = list;
    local_auto_length = length;

	/* Draw image of the field */
    LV_IMG_DECLARE(field);
    lv_obj_t * fieldimg = lv_img_create(lv_scr_act(),0);
    lv_img_set_src(fieldimg, &field);
    lv_obj_align(fieldimg, 0, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_size(fieldimg, 240, 240);

	/* Declare label for later */
	lv_obj_t * label;

	/* Create red style by copying default button style */
	lv_style_copy(&style_red_ina,&lv_style_btn_ina);
	style_red_ina.body.main_color=LV_COLOR_RED;
	style_red_ina.body.grad_color=LV_COLOR_RED;
	style_red_ina.body.border.color = LV_COLOR_BLACK;
	style_red_ina.body.border.width = 5;
	style_red_ina.text.color = LV_COLOR_BLACK;

	/* Create active style in slightly different colors */
	lv_style_copy(&style_red_act,&style_red_ina);
	style_red_act.body.border.color = LV_COLOR_WHITE;

	/* Copy blue style also */
	lv_style_copy(&style_blue_ina,&style_red_ina);
	style_blue_ina.body.main_color=LV_COLOR_BLUE;
	style_blue_ina.body.grad_color=LV_COLOR_BLUE;
	lv_style_copy(&style_blue_act,&style_blue_ina);
	style_blue_act.body.border.color = LV_COLOR_WHITE;

	/* Copy gold finally */
	lv_style_copy(&style_gold_ina,&style_red_ina);
	style_gold_ina.body.main_color=LV_COLOR_YELLOW;
	style_gold_ina.body.grad_color=LV_COLOR_YELLOW;
	lv_style_copy(&style_gold_act,&style_gold_ina);
	style_gold_act.body.border.color = LV_COLOR_WHITE;

	/* Create 5 buttons, 2 red and 2 blue, for each starting position, plus one for skills */
	for(int i = 0; i < 5; i++)
	{
		/* Create button */
		btn_pos[i] = lv_btn_create(lv_scr_act(), NULL);
		lv_btn_set_action(btn_pos[i],LV_BTN_ACTION_CLICK,btn_pos_cb);

		/* Set Style */
		lv_btn_set_style(btn_pos[i],LV_BTN_STYLE_INA,btn_styles[i][0]);
		lv_btn_set_style(btn_pos[i],LV_BTN_STYLE_PR,btn_styles[i][1]);
		lv_btn_set_style(btn_pos[i],LV_BTN_STYLE_REL,btn_styles[i][0]);

		/* Size the button */
		lv_obj_set_size(btn_pos[i], 66, 66);

		/* Align button to corner of the field */
		lv_obj_align(btn_pos[i],fieldimg,btn_align[i],0,0);

		/* Add label */
		label = lv_label_create(btn_pos[i],NULL);
		lv_label_set_text(label,btn_label[i]);
	}

	/* Style for program selection buttons (white) */
	lv_style_copy(&style_white_act,&style_gold_act);
	lv_style_copy(&style_white_ina,&style_gold_ina);

    /* Initially set active auto to -1 to indicate no selection */
    active_auto = -1;
}

/* Run the selected autonomous */
void auto_run(void)
{
    /* If active auto is negative, it's invalid */
    if(active_auto < 0)
    {
        return;
    }

    /* If active auto is greater than the size of the list, it's also invalid */
    if(active_auto >= local_auto_length)
    {
        return;
    }

    /* Get the pointer identified by the active auto, check it's valid, call it */
    auto_func_t function = local_auto_list[active_auto].function;
    if(function)
    {
        function(active_color,active_pos);
    }
}