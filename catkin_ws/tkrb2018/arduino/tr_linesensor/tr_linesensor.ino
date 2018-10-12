#include <ros.h>
#include <std_msgs/Int16.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int16MultiArray.h"

//フォトダイオードの位置とピンを関連付ける
//pinAssign[0]が左端
#define S_NUMBER 8
static const int pinAssign[S_NUMBER] = {0,1,2,3,4,5,6,7};

ros::NodeHandle  nh;
//つくろぼのgithub見て修正
std_msgs::Int16 vals[8];
ros::Publisher sensor_pub;

void setup()
{
  nh.initNode();
  sensornh.advertise(sensor_pub);
 
}

void loop()
{
  sensor_pub.publish(vals);
  nh.spinOnce();
  delay(100);
}
