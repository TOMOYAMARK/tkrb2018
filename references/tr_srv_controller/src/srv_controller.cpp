#include "ros/ros.h"
#include <sstream>
#include "std_msgs/Int32.h"
#include "sensor_msgs/Joy.h"

#define SPINDGR 18;//回転角度単位
int spin = 0;
int spindgr = SPINDGR;
int preInput;
std_msgs::Int32 spinNum;//角度 SPINDGR で何回回転させるか

void joyCallback(const sensor_msgs::Joy::ConstPtr& joy){
  spin = (int)((joy -> (axes[0])*10));
  if(joy -> buttons[1] == 1 && spin != preInput){
    spinNum.data = spin*SPINDGR;
     ROS_INFO("will spin it %d degrees. press the button to confirm",spindgr*spin);
     preInput = spin*SPINDGR;
  }
}

int main(int argc, char **argv){
  ros::init(argc,argv,"srv_controller");
  ros::NodeHandle n;
  ros::Subscriber joy = n.subscribe("joy",1000,joyCallback);
  ros::Publisher servo_pub = n.advertise<std_msgs::Int32>("servo_spin",1000);
  ros::Rate loop_rate(10);

  while(ros::ok()){
    

    //ROS_INFO("will spin it %d degrees %d times. press the button to confirm",spindgr,spin);
    servo_pub.publish(spinNum);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
