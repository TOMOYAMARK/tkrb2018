#include <ros.h>
#include <std_msgs/Int32MultiArray.h>

//フォトダイオードの位置とピンを関連付ける
//pinAssign[0]が左端
static const int pinAssign[] = {4,5,3,6,2,7,1,8};

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
