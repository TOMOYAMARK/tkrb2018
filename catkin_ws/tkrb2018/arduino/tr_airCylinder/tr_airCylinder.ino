#include <ros.h>
#include <std_msgs/Int8.h>

//あとで構造体なりにする
//ぴん
#define PUSH_PIN 1
#define PULL_PIN 2
//ミリ秒　PUSH_PINがHIGHになってから伸び切る時間
#define PUSH_TIME 100

void cylinderCallback(const std_msgs::Int8& msg)
ros::NodeHandle nh;
ros::Subscriber<std_msgs::Int8> cylinder_sub("cylinder_req", cylinderCallback);

void setup() {
  initializeAirCylinder();

  nh.initNode();
  nh.subscribe(cylinder_sub);
  
  pinMode(PUSH_PIN, OUTPUT);
  pinMode(PULL_PIN, OUTPUT);
}

void loop() {
  nh.spinOnce();
}

void cylinderCallback(const std_msgs::Int8& msg) {
  if (msg.data>=0) { //待機時間(ミリ秒)
    delay(msg.data);
    digitalWrite(PUSH_PIN, HIGH);
    delay(PUSH_TIME);
    digitalWrite(PUSH_PIN, LOW);
    digitalWrite(PULL_PIN, HIGH);
  }
}
