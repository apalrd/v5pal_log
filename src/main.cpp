/* Data Logger library for PROS V5
 * Copyright (c) 2022 Andrew Palardy
 * This code is subject to the BSD 2-clause 'Simplified' license
 * See the LICENSE file for complete terms
 */

#include "main.h"
#include "stdio.h"

/* Before including this header, you can optionally define a level
 * to log to. If you do not define this, it will default to WARN
 * 
 * The logger will log all messages of the level defined here or higher
 * 
 * The levels are:
 * LOG_LEVEL_DEBUG (lowest)
 * LOG_LEVEL_INFO
 * LOG_LEVEL_WARN
 * LOG_LEVEL_ERROR
 * LOG_LEVEL_ALWAYS (highest)
 */
#define LOG_LEVEL_FILE LOG_LEVEL_DEBUG
#include "pal/log.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() 
{
	/* Initialize logger - this must be early in your initialization */
	log_init();

	/* Let them know we are in initialize */
	LOG_ALWAYS("In Initialize");
}

/* Get competition mode */
void log_comp_data()
{
	/* To log data, use the log_data_int or log_data_dbl functions
	 * Each takes a string parameter name and int or dbl value
	 *
	 * For the first iteration after log_step() after log_init
	 * or log_segment(), the data will be ignored and the CSV 
	 * header will be generated. For subsequent iterations,
	 * the data will be logged to the CSV file
	 * 
	 * Ensure that you always call log_data in the same order
	 * for each loop iteration, as v5logger does not cache any data
	 * and will write directly to the csv in the order it is called
	 * 
	 * Therefore, any changes to the order of log_data calls will
	 * result in misaligned columns in the CSV file
	 */
	log_data_int("COMP_DISABLED",pros::competition::is_disabled());
	log_data_int("COMP_AUTONOMOUS",pros::competition::is_autonomous());
	log_data_int("COMP_CONNECTED",pros::competition::is_connected());
}

/* Get battery data */
void log_batt_data()
{
	log_data_dbl("BATT_CAP",pros::battery::get_capacity());
	log_data_dbl("BATT_VOLT",pros::battery::get_voltage()/1000.0);
	log_data_dbl("BATT_CUR",pros::battery::get_current()/1000.0);
	log_data_dbl("BATT_TEMP",pros::battery::get_temperature());
}

/* Get control data */
void log_ctrl_data()
{
	/* Keep a controller instance */
	static pros::Controller master(CONTROLLER_MASTER);

	/* Read axes */
	log_data_dbl("CRTL_MSTR_LY",master.get_analog(ANALOG_LEFT_Y));
	log_data_dbl("CRTL_MSTR_LX",master.get_analog(ANALOG_LEFT_X));
	log_data_dbl("CRTL_MSTR_RY",master.get_analog(ANALOG_RIGHT_Y));
	log_data_dbl("CRTL_MSTR_RX",master.get_analog(ANALOG_RIGHT_X));

	/* Read buttons for D-pad */
	log_data_int("CTRL_MSTR_DL",master.get_digital(DIGITAL_LEFT));
	log_data_int("CTRL_MSTR_DR",master.get_digital(DIGITAL_RIGHT));
	log_data_int("CTRL_MSTR_DU",master.get_digital(DIGITAL_UP));
	log_data_int("CTRL_MSTR_DD",master.get_digital(DIGITAL_DOWN));

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

	/* Examples of how to use logging functions */
	if(master.get_digital(DIGITAL_LEFT))
	{
		LOG_DEBUG("Digital Left was pressed!");
	}
	if(master.get_digital(DIGITAL_RIGHT))
	{
		LOG_INFO("Digital Right was pressed!");
	}
	if(master.get_digital(DIGITAL_UP))
	{
		LOG_WARN("Digital Up was pressed!");
	}
	if(master.get_digital(DIGITAL_DOWN))
	{
		LOG_ERROR("Digital Down was pressed!");
	}

	/* Informationally record joystick values to demonstrate logging with parameters
	 * All log macros accept printf type syntax
	 * Additionally, newlines are not required
	 */
	if(master.get_digital(DIGITAL_L1))
	{
		LOG_INFO("Joysticks are x=%d y=%d",master.get_analog(ANALOG_LEFT_X),master.get_analog(ANALOG_LEFT_Y));
	}
}
/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{
	/* log_segment saves the current csv and txt and opens new ones with a new number
	 * This can be used when switching from different code paths which will save different csv data
	 * Since the column order is not guaranteed by v5logger, it is recommended to do this
	 * whenever switching competition modes
	 */
	log_segment();
	/* Let the log know we are disabled */
	LOG_ALWAYS("In Disabled");
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
	LOG_ALWAYS("In Competition Initialize");
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
	LOG_ALWAYS("In Autonomous");
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
		/* log_step MUST be the first call in the loop */
		log_step();
		/* Call some routines which perform robot control and log data */
		log_comp_data();
		log_batt_data();
		log_ctrl_data();
		pros::c::task_delay_until(&prev_time,20);
	}
}
