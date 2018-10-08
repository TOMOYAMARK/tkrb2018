//#define USE_USBCON
#include <ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Int8MultiArray.h>

//フォトダイオードの位置とピンを関連付ける
//pinAssign[0]が左端
#define S_NUMBER 8
static const int pinAssign[S_NUMBER] = {0,1,2,3,4,5,6,7};

ros::NodeHandle  nh;
std_msgs::Int8MultiArray sensor_value;
ros::Publisher linesensor("linesensor", &sensor_value);

void setup()
{
  nh.initNode();
  nh.advertise(linesensor);
}

void loop()
{
  linesensor.publish( &sensor_value );
  for(int i=0; i<S_NUMBER; i++) {
    sensor_value.data[i] = analogRead(pinAssign[i]);
  }
  nh.spinOnce();
  delay(1000);
}
