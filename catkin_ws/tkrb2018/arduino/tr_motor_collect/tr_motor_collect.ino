#include <ros.h>
#include <std_msgs/Int8.h>
#include <Servo.h>

#define CLOSED_ANG 0
#define OPENED_ANG 180

#define STP_CW 40
#define STP_CCW 38
bool current_status = false;
int pulse_count = 0;

void stp_pulse(){
  if(current_status){
    digitalWrite(STP_CW,HIGH);
    current_status = !(current_status);
  }else{
    digitalWrite(STP_CW,LOW);
  }
}


Servo servo_l;
Servo servo_r;

int pos = 0;    //collect part angle 0->close 180->open

void setup() {
  servo_l.attach(A0);  // attaches the servo on pin 9 to the servo object
  servo_r.attach(A1);
}

void servoWriteSync(int ang){//x->collect part angle 0->close 180->open
  //move servos opposite each other with sync
  servo_l.write(ang);
  servo_r.write(180-ang);
}

void closeHand(){
  servoWriteSync(CLOSED_ANG);
}

void openHand(){
  servoWriteSync(OPENED_ANG);
}


void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo_l.write(pos);              // tell servo to go to position in variable 'pos'
    servo_r.write(180-pos);
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    servo_l.write(pos);              // tell servo to go to position in variable 'pos'
    servo_r.write(180-pos);
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

