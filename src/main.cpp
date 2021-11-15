#include "main.h"
#include "stdio.h"
#include "log.h"
#include "pros/apix.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/
static  lv_res_t btn_rel_action(lv_obj_t * btn);
static  lv_res_t ddlist_action(lv_obj_t * ddlist);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create some objects
 */
void lv_tutorial_objects(void)
{

    /********************
     * CREATE A SCREEN
     *******************/
    /* Create a new screen and load it
     * Screen can be created from any type object type
     * Now a Page is used which is an objects with scrollable content*/
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);

    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Object usage demo");  /*Set the text*/
    lv_obj_set_x(label, 50);                        /*Set the x coordinate*/

    /***********************
     * CREATE TWO BUTTONS
     ***********************/
    /*Create a button*/
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
    lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, btn_rel_action); /*Set function to be called when the button is released*/
    lv_obj_align(btn1, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);  /*Align below the label*/

    /*Create a label on the button (the 'label' variable can be reused)*/
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button 1");

    /*Copy the previous button*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn1);        /*Second parameter is an object to copy*/
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 50, 0);    /*Align next to the prev. button.*/

    /*Create a label on the button*/
    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Button 2");

    /****************
     * ADD A SLIDER
     ****************/
    lv_obj_t * slider = lv_slider_create(scr, NULL);                            /*Create a slider*/
    lv_obj_set_size(slider, lv_obj_get_width(lv_scr_act())  / 3, LV_DPI / 3);   /*Set the size*/
    lv_obj_align(slider, btn1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);                /*Align below the first button*/
    lv_slider_set_value(slider, 30);                                            /*Set the current value*/

    /***********************
     * ADD A DROP DOWN LIST
     ************************/
    lv_obj_t * ddlist = lv_ddlist_create(lv_scr_act(), NULL);            /*Create a drop down list*/
    lv_obj_align(ddlist, slider, LV_ALIGN_OUT_RIGHT_TOP, 50, 0);         /*Align next to the slider*/
    lv_obj_set_free_ptr(ddlist, slider);                                   /*Save the pointer of the slider in the ddlist (used in 'ddlist_action()')*/
    lv_obj_set_top(ddlist, true);                                        /*Enable to be on the top when clicked*/
    lv_ddlist_set_options(ddlist, "None\nLittle\nHalf\nA lot\nAll"); /*Set the options*/
    lv_ddlist_set_action(ddlist, ddlist_action);                         /*Set function to call on new option is chosen*/

    /****************
     * CREATE A CHART
     ****************/
    lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
    lv_obj_set_size(chart, lv_obj_get_width(scr) / 2, lv_obj_get_width(scr) / 4);   /*Set the size*/
    lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/
    lv_chart_set_series_width(chart, 3);                                            /*Set the line width*/

    /*Add a RED data series and set some points*/
    lv_chart_series_t * dl1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, dl1, 10);
    lv_chart_set_next(chart, dl1, 25);
    lv_chart_set_next(chart, dl1, 45);
    lv_chart_set_next(chart, dl1, 80);

    /*Add a BLUE data series and set some points*/
    lv_chart_series_t * dl2 = lv_chart_add_series(chart, LV_COLOR_MAKE(0x40, 0x70, 0xC0));
    lv_chart_set_next(chart, dl2, 10);
    lv_chart_set_next(chart, dl2, 25);
    lv_chart_set_next(chart, dl2, 45);
    lv_chart_set_next(chart, dl2, 80);
    lv_chart_set_next(chart, dl2, 75);
    lv_chart_set_next(chart, dl2, 505);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  lv_res_t btn_rel_action(lv_obj_t * btn)
{
    /*Increase the button width*/
    lv_coord_t width = lv_obj_get_width(btn);
    lv_obj_set_width(btn, width + 20);

    return LV_RES_OK;
}

/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  lv_res_t ddlist_action(lv_obj_t * ddlist)
{
    uint16_t opt = lv_ddlist_get_selected(ddlist);      /*Get the id of selected option*/

    lv_obj_t * slider = (lv_obj_t *)lv_obj_get_free_ptr(ddlist);      /*Get the saved slider*/
    lv_slider_set_value(slider, (opt * 100) / 4);       /*Modify the slider value according to the selection*/

    return LV_RES_OK;
}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {

	/* Initialize logger */
	log_init();

	/* Competition mode channels  */
	log_param("comp_dis");
	log_param("comp_auto");
	log_param("comp_conn");

	/* Battery data */
	log_param("batt_cap");
	log_param("batt_volt");
	log_param("batt_cur");
	log_param("batt_temp");

	/* Controller master channels - ints first, then floats */
	log_param("mas_dn");
	log_param("mas_up");
	log_param("mas_lt");
	log_param("mas_rt");
	log_param("mas_a");
	log_param("mas_b");
	log_param("mas_x");
	log_param("mas_y");
	log_param("mas_l1");
	log_param("mas_l2");
	log_param("mas_r1");
	log_param("mas_r2");
	log_param("mas_lx");
	log_param("mas_ly");
	log_param("mas_rx");
	log_param("mas_ry");

	/* IMU channels */
	log_param("imu_cal");
	log_param("imu_hdg");
	log_param("imu_rot");
	log_param("imu_euler_pitch");
	log_param("imu_euler_yaw");
	log_param("imu_euler_roll");
	log_param("imu_acc_x");
	log_param("imu_acc_y");
	log_param("imu_acc_z");

	/* GPS channels */
	log_param("gps_x");
	log_param("gps_y");
	log_param("gps_pitch");
	log_param("gps_yaw");
	log_param("gps_roll");
	log_param("gps_hdg");
	log_param("gps_rot");
	log_param("gps_acc_x");
	log_param("gps_acc_y");
	log_param("gps_acc_z");
	log_param("gps_error");

	/* Encoder channels */
	log_param("left_vel");
	log_param("left_cur");
	log_param("left_pos");
	log_param("left_volt");
	log_param("left_temp");
	log_param("right_vel");
	log_param("right_cur");
	log_param("right_pos");
	log_param("right_volt");
	log_param("right_temp");

	/* Call LV test function */
	lv_tutorial_objects();

}

/* Drive motors */
static pros::Motor drive_left(1,pros::E_MOTOR_GEARSET_18,true,pros::E_MOTOR_ENCODER_ROTATIONS);
static pros::Motor drive_right(10,pros::E_MOTOR_GEARSET_18,false,pros::E_MOTOR_ENCODER_ROTATIONS);

/* Get competition mode */
void log_comp_data()
{
	int data[3];
	data[0] = pros::competition::is_disabled();
	data[1] = pros::competition::is_autonomous();
	data[2] = pros::competition::is_connected();
	log_data(3,data,0,0);
}

/* Get battery data */
void log_batt_data()
{
	double dataDbl[4];
	dataDbl[0] = pros::battery::get_capacity();
	dataDbl[1] = pros::battery::get_voltage();
	dataDbl[2] = pros::battery::get_current();
	dataDbl[3] = pros::battery::get_temperature();
	log_data(0,0,4,dataDbl);
}

/* Get control data */
void log_ctrl_data()
{
	int dataInt[12];
	double dataDbl[4];
	/* Keep a controller instance */
	static pros::Controller master(CONTROLLER_MASTER);

	/* Read axes */
	dataDbl[0] = master.get_analog(ANALOG_LEFT_Y);
	dataDbl[1] = master.get_analog(ANALOG_LEFT_X);
	dataDbl[2] = master.get_analog(ANALOG_RIGHT_Y);
	dataDbl[3] = master.get_analog(ANALOG_RIGHT_X);

	/* Control motors */
	double forward = dataDbl[0];
	double turn = dataDbl[3];
	double left = forward + turn;
	double right = forward - turn;
	drive_left.move(left);
	drive_right.move(right);

	/* Read buttons for D-pad */
	dataInt[0] = master.get_digital(DIGITAL_LEFT);
	dataInt[1] = master.get_digital(DIGITAL_RIGHT);
	dataInt[2] = master.get_digital(DIGITAL_UP);
	dataInt[3] = master.get_digital(DIGITAL_DOWN);

	/* Read buttons for ABXY */
	dataInt[4] = master.get_digital(DIGITAL_A);
	dataInt[5] = master.get_digital(DIGITAL_B);
	dataInt[6] = master.get_digital(DIGITAL_X);
	dataInt[7] = master.get_digital(DIGITAL_Y);

	/* Read Triggers */
	dataInt[8] = master.get_digital(DIGITAL_L1);
	dataInt[9] = master.get_digital(DIGITAL_L2);
	dataInt[10] = master.get_digital(DIGITAL_R1);
	dataInt[11] = master.get_digital(DIGITAL_R2);

	/* Publish data */
	log_data(12,dataInt,4,dataDbl);
}

void log_imu_data()
{
	/* Data arrays */
	int dataInt[1];
	double dataDbl[8];
	/* IMU */
	static pros::IMU emu(7);

	/* Get is_calibrating */
	dataInt[0] = emu.is_calibrating();

	/* Get Rot and Heading */
	dataDbl[0] = emu.get_heading();
	dataDbl[1] = emu.get_rotation();

	/* Get Euler angles */
    pros::c::euler_s_t euler = emu.get_euler();
	dataDbl[2] = euler.pitch;
	dataDbl[3] = euler.yaw;
	dataDbl[4] = euler.roll;

	/* Get Accels */
	pros::c::imu_gyro_s_t accel = emu.get_accel();
	dataDbl[5] = accel.x;
	dataDbl[6] = accel.y;
	dataDbl[7] = accel.z;

	/* Log data */
	log_data(1,dataInt,8,dataDbl);
}

/* Log GPS Data */
void log_gps_data()
{
	/* Data arrays */
	double dataDbl[11];

	/* Static GPS */
	static pros::GPS gps(8);

	/* Get Status */
	pros::c::gps_status_s_t status = gps.get_status();
	dataDbl[0] = status.x;
	dataDbl[1] = status.y;
	dataDbl[2] = status.pitch;
	dataDbl[3] = status.yaw;
	dataDbl[4] = status.roll;

	/* Heading and rotation */
	dataDbl[5] = gps.get_heading();
	dataDbl[6] = gps.get_rotation();

	/* Accles */
	pros::c::gps_accel_s_t accel = gps.get_accel();
	dataDbl[7] = accel.x;
	dataDbl[8] = accel.y;
	dataDbl[9] = accel.z;

	/* Precision */
	dataDbl[10] = gps.get_error();

	/* Publish data */
	log_data(0,0,11,dataDbl);
}

/* Log motor data */
void log_motor_data()
{
	double dataDbl[10];

	/* Left motor data */
	dataDbl[0] = drive_left.get_actual_velocity();
	dataDbl[1] = drive_left.get_current_draw();
	dataDbl[2] = drive_left.get_position();
	dataDbl[3] = drive_left.get_voltage();
	dataDbl[4] = drive_left.get_temperature();

	/* right motor data */
	dataDbl[5] = drive_right.get_actual_velocity();
	dataDbl[6] = drive_right.get_current_draw();
	dataDbl[7] = drive_right.get_position();
	dataDbl[8] = drive_right.get_voltage();
	dataDbl[9] = drive_right.get_temperature();

	/* Log data */
	log_data(0,0,10,dataDbl);
}
/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{
	while(1)
	{
		/* read axes and buttons */
		log_step();
		log_comp_data();
		pros::delay(20);
	}
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize()
{
	while(1)
	{
		/* read axes and buttons */
		pros::delay(20);
	}
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() 
{
	while(1)
	{
		/* read axes and buttons */
		log_step();
		pros::delay(20);
	}
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() 
{
	while(1)
	{
		/* read axes and buttons */
		log_step();
		log_comp_data();
		log_ctrl_data();
		log_imu_data();
		log_gps_data();
		log_motor_data();
		pros::delay(20);
	}
}
