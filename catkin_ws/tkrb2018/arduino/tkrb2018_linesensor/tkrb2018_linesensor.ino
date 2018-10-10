#include <ros.h>
#include <std_msgs/Int16.h>

//フォトダイオードの位置とピンを関連付ける
//pinAssign[0]が左端
#define S_NUMBER 8
static const int pinAssign[S_NUMBER] = {0,1,2,3,4,5,6,7};

ros::NodeHandle  nh;
std_msgs::Int16 msgs[8] = {};
ros::Publisher pub0("ls0", &msgs[0]);
ros::Publisher pub1("ls1", &msgs[1]);
ros::Publisher pub2("ls2", &msgs[2]);
ros::Publisher pub3("ls3", &msgs[3]);
ros::Publisher pub4("ls4", &msgs[4]);
ros::Publisher pub5("ls5", &msgs[5]);
ros::Publisher pub6("ls6", &msgs[6]);
ros::Publisher pub7("ls7", &msgs[7]);
ros::Publisher pub[8] = {pub0, pub1, pub2, pub3, pub4, pub5, pub6, pub7};

void setup()
{
  nh.initNode();
  for(int i=0; i<S_NUMBER; i++)
    nh.advertise(pub[i]);
}

void loop()
{
  for(int i=0; i<S_NUMBER; i++) {
    msgs[i].data = analogRead(pinAssign[i]);
    pub[i].publish(&msgs[i]);
  }
  nh.spinOnce();
  delay(100);
}
