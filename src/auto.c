

#include "pros/apix.h"
#include "display/lvgl.h"

static int active_mode;

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

/* Styles associated with each starting position button in the inactive and active states */
static lv_style_t * btn_styles[5][2] = 
{
	{&style_red_ina, &style_red_act},
	{&style_red_ina, &style_red_act},
	{&style_blue_ina, &style_blue_act},
	{&style_blue_ina, &style_blue_act},
	{&style_gold_ina, &style_gold_act}
};

/* Button callback */
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
    return LV_RES_OK;
}


void auto_picker(void)
{
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
}