#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Int8.h"
//#include "tr_controller/motor.h"
#include <sstream>

#define THRESHOLD 0.7
#define DEFAULTPW 50
#define WHEEL_DIAMETER 87.5 * 2   //mm
#define INNER_WHEEL_DISTANCE 585  //mm
#define PI 3.14 //pi
#define PULSE_PER_STEP 1.8 //degree
#define ONE_PULSE_DISTANCE WHEEL_DIAMETER * PI * PULSE_PER_STEP / 360 //mm
#define ONE_GRID_DISTANCE 300 //mm
#define LINE_TRACE_GAIN 15 //xxx
#define DOWN_LINIA_PW -100
#define UP_LINIA_PW 100
#define DOWN_NUNO_PW -95
#define UP_NUNO_PW 95
#define LINIA_DEFAULT_PULSE 47000//46000
#define NUNO_DEFAULT_PULSE 24500//22500
#define LINIA_PULY_D 60 //diameter mm (tyokkei)
#define LINIA_UP_DISTANCE 900 //mm xxx
#define LINIA_ONE_PULSE_STEP 0.24 //xxx degree
#define LINIA_UP_PULSE 47000//LINIA_UP_DISTANCE / ((LINIA_PULY_D * 3.14 * LINIA_ONE_PULSE_STEP) / 360)
#define NUNO_PULY_D 30 //diameter mm (tyokkei)
#define NUNO_UP_DISTANCE 900 //mm xxx
#define NUNO_ONE_PULSE_STEP 1.8 //xxx degree
#define NUNO_UP_PULSE 20000//18000//20000//NUNO_UP_DISTANCE / ((NUNO_PULY_D * 3.14 * NUNO_ONE_PULSE_STEP) / 360)
#define NUNO_PULL_PULSE 6500//6000//4500//3827//xxx
#define STATE_SETUP 7
#define STATE_RESET_NUNO 8
#define STATE_UP_LINIA 11
#define STATE_PULL_NUNO 12


int pulse_motor_linia, pulse_motor_nuno;
std_msgs::Int8 power_motor_linia, power_motor_nuno;
int command;
int linia_target_pulse, nuno_target_pulse;
int state = 0; // 1 is working, 0 is waiting.

/*
0: pass
1: down_linia
2: stop_linia
3: down_nuno
4: stop_nuno
5: up
6: pull nuno
8: reset nuno
7: setup
*/
void pulseMotorLiniaCallback(const std_msgs::Int32::ConstPtr& msg) {
  pulse_motor_linia = msg -> data;
}

void pulseMotorNunoCallback(const std_msgs::Int32::ConstPtr& msg) {
  pulse_motor_nuno = msg -> data;
}

void submotorCommandCallback(const std_msgs::Int8::ConstPtr& msg) {
  command = msg -> data;
  if (command == 1) { // down_linia
    power_motor_linia.data = DOWN_LINIA_PW;
  } else if (command == 2) { // stop down_linia
    power_motor_linia.data = 0;    
  } else if (command == 3) { // down nuno
    power_motor_nuno.data = DOWN_NUNO_PW;    
  } else if (command == 4) { // stop down nuno
    power_motor_nuno.data = 0;
  } else if (command == 5) { // up linia and nuno
    linia_target_pulse = pulse_motor_linia + LINIA_UP_PULSE;
    //nuno_target_pulse = pulse_motor_nuno + NUNO_UP_PULSE;
    ROS_INFO("linia_target_pulse:%d\n", linia_target_pulse);
    //ROS_INFO("nuno_target_pulse:%d\n", nuno_target_pulse);    
    power_motor_linia.data = UP_LINIA_PW;
    //power_motor_nuno.data = UP_NUNO_PW;
    state = STATE_UP_LINIA;
  } else if (command == 6) { // pull nuno
    nuno_target_pulse = pulse_motor_nuno - NUNO_PULL_PULSE;
    power_motor_nuno.data = DOWN_NUNO_PW;
    power_motor_linia.data = 0;    
    state = STATE_PULL_NUNO;    
  } else if (command == 7) {// setup
    nuno_target_pulse = pulse_motor_nuno - NUNO_DEFAULT_PULSE;
    linia_target_pulse = pulse_motor_nuno - LINIA_DEFAULT_PULSE;        power_motor_linia.data = DOWN_LINIA_PW;
    power_motor_nuno.data = DOWN_NUNO_PW;
    state = STATE_SETUP;
  } else if (command == 8) { // reset nuno
    nuno_target_pulse = pulse_motor_nuno + NUNO_PULL_PULSE;
    power_motor_nuno.data = UP_NUNO_PW;
    power_motor_linia.data = 0;    
    state = STATE_RESET_NUNO;    
  }
}
  
void command_handler() {
  if (state == STATE_UP_LINIA) {//up linia
    if (pulse_motor_linia > linia_target_pulse) {
      power_motor_linia.data = 0;      
    }
    //if (pulse_motor_nuno > nuno_target_pulse) {
    //  power_motor_nuno.data = 0;
    //}
    if (pulse_motor_linia > linia_target_pulse) {
      // end
      power_motor_linia.data = 0;
      //power_motor_nuno.data = 0;        
      state = 0;
    }
  }
  if (state == STATE_PULL_NUNO) {
    if (pulse_motor_nuno < nuno_target_pulse) {
      power_motor_linia.data = 0;          
      power_motor_nuno.data = 0;
      state = 0;
    }    
  }
  if (state == STATE_RESET_NUNO) {
    if (pulse_motor_nuno > nuno_target_pulse) {
      power_motor_linia.data = 0;          
      power_motor_nuno.data = 0;
      state = 0;
    }    
  }
  
  if (state == STATE_SETUP) {
    if (pulse_motor_linia < linia_target_pulse) {
      power_motor_linia.data = 0;      
    }
    if (pulse_motor_nuno < nuno_target_pulse) {
      power_motor_nuno.data = 0;
    }
    if (pulse_motor_linia < linia_target_pulse && pulse_motor_nuno < nuno_target_pulse) {
      // end
      power_motor_linia.data = 0;
      power_motor_nuno.data = 0;        
      state = 0;
    }
    
  }
  
}
  
int main(int argc, char **argv)
{
  ros::init(argc, argv, "tr_motor_sub");
  ros::NodeHandle n;
  ros::Publisher motor_linia_pub = n.advertise<std_msgs::Int8>("power_motor_linia",1000);
  ros::Publisher motor_nuno_pub = n.advertise<std_msgs::Int8>("power_motor_nuno",1000);
  ros::Subscriber pulse_motor_linia_sub = n.subscribe("pulse_motor_linia",1000, pulseMotorLiniaCallback);
  ros::Subscriber pulse_motor_nuno_sub = n.subscribe("pulse_motor_nuno",1000, pulseMotorNunoCallback);
  ros::Subscriber command_sub = n.subscribe("sub_motor_command", 1000, submotorCommandCallback);
  ros::Rate loop_rate(10);

  int count = 0;
  while (ros::ok())
  {
    command_handler();
    motor_linia_pub.publish(power_motor_linia);
    motor_nuno_pub.publish(power_motor_nuno);
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
  }


  return 0;
}
