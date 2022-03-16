#include "main.h"
#include "stdio.h"
#include "log.h"
#include "auto.h"

/* Auto functions */
void auto_skills_1(auto_color_t color,auto_pos_t pos)
{
	LOG_ALWAYS("I am Auto Skills 1, color is %d, pos is %d",color,pos);
}
void auto_skills_2(auto_color_t color,auto_pos_t pos)
{
	LOG_ALWAYS("I am Auto Skills 2, color is %d, pos is %d",color,pos);
}
void auto_match_p1_1(auto_color_t color,auto_pos_t pos)
{
	LOG_ALWAYS("I am Auto Match P1 1, color is %d, pos is %d",color,pos);
}
void auto_match_p1_2(auto_color_t color,auto_pos_t pos)
{
	LOG_ALWAYS("I am Auto Match P1 2, color is %d, pos is %d",color,pos);
}
void auto_match_p2_1(auto_color_t color,auto_pos_t pos)
{
	LOG_ALWAYS("I am Auto Match P2 1, color is %d, pos is %d",color,pos);
}
void auto_match_p2_2(auto_color_t color,auto_pos_t pos)
{
	LOG_ALWAYS("I am Auto Match P2 2, color is %d, pos is %d",color,pos);
}


/* List of autonomous routines */
const auto_routine_t auto_list[] =
{
	/* Robot skills options */
	{ auto_skills_1, AUTO_POS_SKILLS, "Skills 1"},
	{ auto_skills_2, AUTO_POS_SKILLS, "Skills 2"},
	{ auto_skills_1, AUTO_POS_SKILLS, "Skills 3"},
	{ NULL, AUTO_POS_SKILLS, "Skills 4"},
	/* Match autos */
	{ auto_match_p1_1, AUTO_POS_1, "Match P1 #1"},
	{ auto_match_p1_2, AUTO_POS_1, "Match P1 #2"},
	{ auto_match_p2_1, AUTO_POS_2, "Match P2 #1"},
	{ auto_match_p2_2, AUTO_POS_2, "Match P2 #2"},
};


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() 
{
	/* Initialize logger */
	log_init();

	LOG_INFO("In Initialize");

	/* Call Auto Picker */
	auto_picker(auto_list, sizeof(auto_list)/sizeof(auto_routine_t));

}

/* Drive motors */
static pros::Motor drive_left(1,pros::E_MOTOR_GEARSET_18,true,pros::E_MOTOR_ENCODER_ROTATIONS);
static pros::Motor drive_right(10,pros::E_MOTOR_GEARSET_18,false,pros::E_MOTOR_ENCODER_ROTATIONS);

/* Get competition mode */
void log_comp_data()
{
	log_data_int("COMP_DISABLED",pros::competition::is_disabled());
	log_data_int("COMP_AUTONOMOUS",pros::competition::is_autonomous());
	log_data_int("COMP_CONNECTED",pros::competition::is_connected());
}

/* Get battery data */
void log_batt_data()
{
	log_data_dbl("BATT_CAP",pros::battery::get_capacity());
	log_data_dbl("BATT_VOLT",pros::battery::get_voltage());
	log_data_dbl("BATT_CUR",pros::battery::get_current());
	log_data_dbl("BATT_TEMP",pros::battery::get_temperature());
}

/* Get control data */
void log_ctrl_data()
{
	/* Keep a controller instance */
	static pros::Controller master(CONTROLLER_MASTER);

	/* Read axes */
	log_data_dbl,("CRTL_MSTR_LY",master.get_analog(ANALOG_LEFT_Y));
	log_data_dbl,("CRTL_MSTR_LX",master.get_analog(ANALOG_LEFT_X));
	log_data_dbl,("CRTL_MSTR_RY",master.get_analog(ANALOG_RIGHT_Y));
	log_data_dbl,("CRTL_MSTR_RX",master.get_analog(ANALOG_RIGHT_X));

	/* Control motors */
	double forward = master.get_analog(ANALOG_LEFT_Y);
	double turn = master.get_analog(ANALOG_RIGHT_X);
	double left = forward + turn;
	double right = forward - turn;
	drive_left.move(left);
	drive_right.move(right);

	/* Read buttons for D-pad */
	log_data_int("CTRL_MSTR_DL",master.get_digital(DIGITAL_LEFT));
	log_data_int("CTRL_MSTR_DL",master.get_digital(DIGITAL_RIGHT));
	log_data_int("CTRL_MSTR_DL",master.get_digital(DIGITAL_UP));
	log_data_int("CTRL_MSTR_DL",master.get_digital(DIGITAL_DOWN));

	/* Read buttons for ABXY */
	log_data_int("CTRL_MSTR_DA",master.get_digital(DIGITAL_A));
	log_data_int("CTRL_MSTR_DB",master.get_digital(DIGITAL_B));
	log_data_int("CTRL_MSTR_DX",master.get_digital(DIGITAL_X));
	log_data_int("CTRL_MSTR_DY",master.get_digital(DIGITAL_Y));

	/* Read Triggers */
	log_data_int("CTRL_MSTR_L1",master.get_digital(DIGITAL_L1));
	log_data_int("CTRL_MSTR_L2",master.get_digital(DIGITAL_L2));
	log_data_int("CTRL_MSTR_R1",master.get_digital(DIGITAL_R1));
	log_data_int("CTRL_MSTR_R2",master.get_digital(DIGITAL_R2));
}

void log_imu_data()
{
	/* IMU */
	static pros::IMU emu(7);

	/* Get is_calibrating */
	log_data_int("IMU_CAL",emu.is_calibrating());

	/* Get Rot and Heading */
	log_data_dbl,("IMU_HDG",emu.get_heading());
	log_data_dbl,("IMU_ROT",emu.get_rotation());

	/* Get Euler angles */
    pros::c::euler_s_t euler = emu.get_euler();
	log_data_dbl,("IMU_PITCH",euler.pitch);
	log_data_dbl,("IMU_YAW",euler.yaw);
	log_data_dbl,("IMU_ROLL",euler.roll);

	/* Get Accels */
	pros::c::imu_gyro_s_t accel = emu.get_accel();
	log_data_dbl,("IMU_ACC_X",accel.x);
	log_data_dbl,("IMU_ACC_Y",accel.y);
	log_data_dbl,("IMU_ACC_Z",accel.z);
}

/* Log GPS Data */
void log_gps_data()
{
	/* Static GPS */
	static pros::GPS gps(8);

	/* Get Status */
	pros::c::gps_status_s_t status = gps.get_status();
	log_data_dbl,("GPS_STS_X",status.x);
	log_data_dbl,("GPS_STS_Y",status.y);
	log_data_dbl,("GPS_PITCH",status.pitch);
	log_data_dbl,("GPS_YAW",status.yaw);
	log_data_dbl,("GPS_ROLL",status.roll);

	/* Heading and rotation */
	log_data_dbl,("GPS_HDG",gps.get_heading());
	log_data_dbl,("GPS_ROT",gps.get_rotation());

	/* Accles */
	pros::c::gps_accel_s_t accel = gps.get_accel();
	log_data_dbl,("GPS_ACC_X",accel.x);
	log_data_dbl,("GPS_ACC_Y",accel.y);
	log_data_dbl,("GPS_ACC_Z",accel.z);

	/* Precision */
	log_data_dbl,("GPS_ERROR",gps.get_error());
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
	//log_data(0,0,10,dataDbl);
}
/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{
	log_segment();
	LOG_ERROR("In Disabled");
	//opcontrol();
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
	log_segment();
	LOG_ERROR("In Competition Initialize");
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
	/* Create a new log in auto */
	log_segment();
	LOG_ERROR("In Autonomous");
	auto_run();
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
	/* Create new log in opcontrol */
	log_segment();
	LOG_ERROR("In Opcontrol");
	static uint32_t prev_time = pros::c::millis();
	while(1)
	{
		/* log_step MUST be first */
		log_step();
		/* read axes and buttons */
		log_comp_data();
		log_batt_data();
		log_ctrl_data();
		log_imu_data();
		log_gps_data();
		log_motor_data();
		LOG_DEBUG("The task has run again");
		pros::c::task_delay_until(&prev_time,20);
	}
}
