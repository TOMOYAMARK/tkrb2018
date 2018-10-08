#include <ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Int8MultiArray.h>
#include <std_msgs/MultiArrayDimension.h>

//フォトダイオードの位置とピンを関連付ける
//pinAssign[0]が左端
#define S_NUMBER 8
static const int pinAssign[S_NUMBER] = {0,1,2,3,4,5,6,7};

ros::NodeHandle  nh;
std_msgs::Int8MultiArray msg;
ros::Publisher linsensor_publisher("linsensor_publisher", &msg);

void setup()
{
  nh.initNode();
  msg.layout.dim=(std_msgs::MultiArrayDimension *)
  malloc(sizeof(std_msgs::MultiArrayDimension)*2);
  msg.layout.dim[0].size=sizeof(int[8]);
  msg.layout.dim[0].stride=8;
  msg.layout.dim[0].label="line_sensor";
  msg.layout.data_offset = 0;
  msg.layout.dim_length=1;
  msg.data_length=sizeof(int[8]);
  nh.advertise(linsensor_publisher);
}

void loop()
{
  for(int i=0; i<S_NUMBER; i++) {
    msg.data[i] = analogRead(pinAssign[i]);
  }
  linsensor_publisher.publish(&msg);
  nh.spinOnce();
  delay(1000);
}
