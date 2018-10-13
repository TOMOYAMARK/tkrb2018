#include <ros.h>
#include <std_msgs/Int32MultiArray.h>

//フォトダイオードの位置とピンを関連付ける
//pinAssign[0]が左端
#define S_NUMBER 8
static const int pinAssign[S_NUMBER] = {1,2,3,4,5,6,7,8};

ros::NodeHandle  nh;
//つくろぼのgithub見て修正
std_msgs::Int32MultiArray msgs;
ros::Publisher pub("ls", &msgs);
long ls[8] = {};

void setup()
{
  nh.initNode();
  nh.advertise(pub);
}

void loop()
{
  msgs.data_length=8;
  for(int i=0;i<8;i++){
    ls[i] = analogRead(pinAssign[i]);
  }
  msgs.data = ls;
  pub.publish(&msgs);
  nh.spinOnce();
}
