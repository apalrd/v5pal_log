#include "main.h"
#include "stdio.h"
#include "log.h"
#include "auto.h"

/* Auto functions */
void auto_skills_1(auto_color_t,auto_pos_t)
{

}
void auto_skills_2(auto_color_t,auto_pos_t)
{

}
void auto_match_p1_1(auto_color_t,auto_pos_t)
{

}
void auto_match_p1_2(auto_color_t,auto_pos_t)
{

}
void auto_match_p2_1(auto_color_t,auto_pos_t)
{

}
void auto_match_p2_2(auto_color_t,auto_pos_t)
{

}


/* List of autonomous routines */
const auto_routine_t auto_list[] =
{
	/* Robot skills options */
	{ auto_skills_1, AUTO_POS_SKILLS, "Skills 1"},
	{ auto_skills_2, AUTO_POS_SKILLS, "Skills 2"},
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
	printf("Begun Init\n");

	/* Initialize logger */
	log_init();

	LOG_INFO("In Initialize");

#if 0
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
	log_param("gps_gyro_x");
	log_param("gps_gyro_y");
	log_param("gps_gyro_z");

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
#endif
	/* Call LV test function */
	//auto_picker(auto_list, sizeof(auto_list)/sizeof(auto_routine_t));

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
	//log_data(3,data,0,0);
}

/* Get battery data */
void log_batt_data()
{
	double dataDbl[4];
	dataDbl[0] = pros::battery::get_capacity();
	dataDbl[1] = pros::battery::get_voltage();
	dataDbl[2] = pros::battery::get_current();
	dataDbl[3] = pros::battery::get_temperature();
	//log_data(0,0,4,dataDbl);
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
	//log_data(12,dataInt,4,dataDbl);
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
	//log_data(1,dataInt,8,dataDbl);
}

/* Log GPS Data */
void log_gps_data()
{
	/* Data arrays */
	double dataDbl[14];

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

	/* Gyro rates */
	//pros::c::gps_gyro_s_t gyro = gps.get_gyro_rate();
	//dataDbl[11] = gyro.x;
	//dataDbl[12] = gyro.y;
	//dataDbl[13] = gyro.z;

	/* Publish data */
	//log_data(0,0,14,dataDbl);

	#if 0
	printf("GPS x=%1.3f y=%1.3f h=%3.1f r=%2.3f e=%3.3f pt%2.1f yw%3.1f rl%2.1f", 
			dataDbl[0],
			dataDbl[1],
			dataDbl[5],
			dataDbl[6],
			dataDbl[10],
			dataDbl[2],
			dataDbl[3],
			dataDbl[4]);
	printf("ax%0.2f ay%0.2f az%0.2f gx%f gy%f gz%f\n",
			dataDbl[7],
			dataDbl[8],
			dataDbl[9],
			dataDbl[11],
			dataDbl[12],
			dataDbl[13]);
	#endif
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
	printf("In disabled\n");
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
	printf("In competition_initialize\n");
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
	printf("In autonomous\n");
	LOG_ERROR("In Autonomous");
	//opcontrol();
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
	printf("In opcontrol\n");
	LOG_ERROR("In Opcontrol");
	static uint32_t prev_time = pros::c::millis();
	while(1)
	{
		/* read axes and buttons */
		//log_step();
		log_comp_data();
		log_batt_data();
		log_ctrl_data();
		log_imu_data();
		log_gps_data();
		log_motor_data();
		LOG_DEBUG("The task has run again");
		pros::c::task_delay_until(&prev_time,20);
		return;
	}
}
