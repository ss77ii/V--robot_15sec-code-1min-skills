#include "../include/main.h"
#include "GarglonConfiguration.hpp"

int clawState = 0; // 0 = retracted

double sign(double x){
	if(x > 0){
	  return 1;
	}
	if (x < 0){
	  return -1;
	}
	return 0;
}

void drive_control_fn(void* param)
{
	float analog_left_y = 0;
	float analog_right_y = 0;
	float left_output = 0;
	float right_output = 0;
	float power_scalar = 2;
	while(true)
    {
		if(master.get_digital(DIGITAL_UP) == 1)
        {
			left_front_motor.move(127);
			left_back_motor.move(127);
			left_mid_motor.move(127);
			right_front_motor.move(127);
			right_back_motor.move(127);
			right_mid_motor.move(127);
		}
		else if(master.get_digital(DIGITAL_DOWN) == 1)
        {
			left_front_motor.move(-127);
			left_back_motor.move(-127);
			left_mid_motor.move(-127);
			right_front_motor.move(-127);
			right_back_motor.move(-127);
			right_mid_motor.move(-127);
		}
		else if(master.get_digital(DIGITAL_RIGHT) == 1)
        {
			left_front_motor.move(60);
			left_back_motor.move(60);
			left_mid_motor.move(60);
			right_front_motor.move(60);
			right_back_motor.move(60);
			right_mid_motor.move(60);
		}
		else
        {
			analog_left_y = master.get_analog(ANALOG_LEFT_Y);
			left_output = sign(analog_left_y) * fabs(pow(analog_left_y, power_scalar)/pow(127, power_scalar - 1));

			analog_right_y = master.get_analog(ANALOG_RIGHT_Y);
			right_output = sign(analog_right_y) * fabs(pow(analog_right_y, power_scalar)/pow(127, power_scalar - 1)); 
			
			left_front_motor.move(left_output);
			left_back_motor.move(left_output);
			left_mid_motor.move(left_output);
			right_front_motor.move(right_output);
			right_back_motor.move(right_output);
			right_mid_motor.move(right_output);
		}
		pros::delay(10);
	}
}


void intake_control_fn(void* param)
{
	int buttonStateR2 = 0; // 0 = not pressed
	int intakeState = 0; // 0 = retracted
	int speed = 110;
	int buttonStateXY = '0';
	while(true)
    {
		if (master.get_digital(DIGITAL_X) == 1){
			intake_motor.move(90);
		}
		else if (master.get_digital(DIGITAL_Y) == 1){
			intake_motor.move(110);
		}
		else if (master.get_digital(DIGITAL_R2) == 1)
        {
			if(buttonStateR2 == 0)
            {
				if(intakeState == 0)
                {
					intakeState = 1;
					intake_motor.move(speed);
				}
				else if(intakeState == 1)
                {
					intakeState = 0;
					intake_motor.move(0);
				}
			}
			buttonStateR2 = 1;
		}
		else
        {
			buttonStateR2 = 0;
		}
		delay(20);
	}
}

void arm_control_fn(void* param)
{
	while(true)
    {
  	if (master.get_digital(DIGITAL_L2) == 1 && master.get_digital(DIGITAL_L1) == 0)
        {
			arm_motor.move(-127);
 		}
		else if (master.get_digital(DIGITAL_L2) == 0 && master.get_digital(DIGITAL_L1) == 1)
        {
			arm_motor.move(127);
		}
		else
    {
			arm_motor.move(0);
		}
		pros::delay(10);
	}
}

void claw_control_fn(void* param)
{
	int buttonStateR1 = 0; // 0 = not pressed
	int clear = 0; // 0 = active pushing
	while(true)
    {
		if (master.get_digital(DIGITAL_R1) == 1)
        {
			if(buttonStateR1 == 0)
            {
				if(clawState == 0)
                {
					clawState = 1;
					front_piston.set_value(true);
					clear = 1;
				}
				else if(clawState == 1)
                {
					clawState = 0;
					clear = 0;
					front_piston.set_value(false);
				}
			}
			buttonStateR1 = 1;
		}
		else
        {
			buttonStateR1 = 0;
		}
		pros::delay(10);
	}
}

void high_goal_control_fn(void* param)
{
	int buttonStateR1 = 0; // 0 = not pressed
	int clawState = 0; // 0 = retracted
	int clear = 0; // 0 = active pushing
	while(true)
    {
		if (master.get_digital(DIGITAL_B) == 1)
        {
			if(buttonStateR1 == 0)
            {
				if(clawState == 0)
                {
					clawState = 1;
					top_piston.set_value(true);
					clear = 1;
				}
				else if(clawState == 1)
                {
					clawState = 0;
					clear = 0;
					top_piston.set_value(false);
				}
			}
			buttonStateR1 = 1;
		}
		else
        {
			buttonStateR1 = 0;
		}
		pros::delay(10);
	}
}


	void hook_control_fn(void* param)
    {
		int buttonStateA = 0; // 0 = not pressed
		int hookState = 0; // 0 = retracted

		while(true)
        {
			if (master.get_digital(DIGITAL_A) == 1)
            {
				if(buttonStateA == 0)
                {
					if(hookState == 0)
                    {
						hookState = 1;
						back_piston.set_value(true);
					}
					else if(hookState == 1)
                    {
						hookState = 0;
						back_piston.set_value(false);
					}
				}
				buttonStateA = 1;
			}
			else
            {
				buttonStateA = 0;
			}
			pros::delay(10);
		}
	}


/*********************************************************************************************************
 *     The following is the general behaviours, not very correct for remote competition.
 *
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
 ***********************************************************************************************************/
void opcontrol()
{
	bool run_skills = true;


	// left_front_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	// left_back_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	// right_front_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	// right_back_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

/*************************************************************************************************************************
* ??CONTROLLER FUNCTIONS
* ??In teamwork competition, the remote system will load both auton and opControl program together after initialization.
* ??then the system will execute the 15 seconds auton program. After the Auton finishes, the system will control the
* ??execution of opControl. In this situation, the three tasks in the opControl loaded in the system will conflict
* ??with the tasks used for 15-sec Auton and the Auton will fail. ?? ??
* ??
* ??So we have to add the while loop below to prevent the three tasks from??loading during the system loads Auton and
* ??opControl. After Auton finished and opControl is enabled, we press the DIGITAL_DOWN button to break the while loop ??
* ??and then loads the three tasks used for opControl.
************************************************************************************************************************/


// IMPORTANT //
// IMPORTANT //
// IMPORTANT //

//  MUST BE UNCOMMENTED IF YOU ARE TO RUN TEAMWORK MATCHES  //

	left_front_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	left_back_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	left_mid_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	right_front_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	right_back_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	right_mid_motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	intake_motor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
	arm_motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
/*
	while (true){
		if (master.get_digital(DIGITAL_LEFT) == 1){
			sys_initial_robot_heading = 180;

			back_piston.set_value(true);
			delay(500);
			back_piston.set_value(false);
			delay(200);
			clawAction_1 = {0, false, 1};
			hookAction_1 = {0, false, 1};
			arm_motor.move(50);
		// ------------------------------------------------------------------------------------ //
		//// part 1 take first blue goal
			goStraightCmPID_lib(5.5, 180, 80, MOVE_BACKWARD, 0, 0, 0, 5, 0, 0, 500, 1, hardwareParameter);
			delay(250);
			hookAction_1 = {0, true, 1};
			delay(100);
			break;
		}
		else if (master.get_digital(DIGITAL_RIGHT) == 1){
			break;
		}
		delay(20);
	}
*/


sys_initial_to_auton_drifting = inertial_sensor.get_rotation();
sys_initial_robot_heading = 180;
	goStraightCmPID_lib(5.5, 180, 80, MOVE_BACKWARD, 0, 0, 0, 5, 0, 0, 500, 1, hardwareParameter);
	hookAction_1 = {0, true, 1};
	delay(100);
//	goStraightCmPID_lib(60, 77, 127, MOVE_FORWARD, 2.0, 0, 2.5, 1, 0, 0, 1500, 1, hardwareParameter); //100,80,127


	intake_set.remove();
	hook_set.remove();
	claw_set.remove();
	arm_set.remove();


	pros::Task drive(drive_control_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "drive control");
	pros::Task intake(intake_control_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "intake control");
	pros::Task arm(arm_control_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "arm control");
	pros::Task claw(claw_control_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "claw control");
	pros::Task hook(hook_control_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "hook control");
	pros::Task highgoal(high_goal_control_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "high goal control");

	double angle;

	while(true) {
		pros::lcd::print(0, "RF:%.1f-RM:%.1f-RB:%.1f", right_front_motor.get_temperature(), right_mid_motor.get_temperature(), right_back_motor.get_temperature());
		pros::lcd::print(1, "LF:%.1f-LM:%.1f-LB:%.1f", left_front_motor.get_temperature(), left_mid_motor.get_temperature(), left_back_motor.get_temperature());
		pros::lcd::print(2, "ARM:%.1f", arm_motor.get_temperature());
		if (master.get_digital(DIGITAL_LEFT)){
			arm.suspend();
			drive.suspend();
			claw.suspend();
			arm_motor.move_absolute(PRESS_BRIDGE + 10, 127);
			delay(100);
			angle = inertial_sensor.get_rotation();
			front_piston.set_value(false);
			clawState = 0;
			delay(100);
			goStraightCmPID_lib(5, angle, 80, MOVE_BACKWARD, 0, 0, 0, 5, 0, 0, 500, 1, hardwareParameter);
			arm_motor.move_absolute(PRESS_BRIDGE - 100, 127);
			delay(200);
			claw.resume();
			arm.resume();
			drive.resume();
			delay(100);
		}

		pros::delay(20);
	}
}
