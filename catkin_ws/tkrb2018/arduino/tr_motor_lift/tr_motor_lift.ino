/* 
 * rosserial::std_msgs::Float64 Test
 * Receives a Float64 input, subtracts 1.0, and publishes it
 */

#include <ros.h>
#include <std_msgs/Int8.h>

#define STEPPING_MOTOR_SUM 1
#define WHEEL_DIAMETER 200 
#define STEP 1.8
#define DEFAULTPW 100
#define ONE_GRID_PULSE 2000 //XXX
#define PULSE_FOWARD (WHEEL_DIAMETER * 3.14 * STEP / 360)
#define LIFT_PW 60

ros::NodeHandle nh;

float x; 
int target_pulse = 0;

// stepping motor
static const int motor_cw[STEPPING_MOTOR_SUM] = {52};
static const int motor_ccw[STEPPING_MOTOR_SUM] = {50};
static const int motor_min_low_time[STEPPING_MOTOR_SUM] = {200};
static const int motor_max_low_time[STEPPING_MOTOR_SUM] = {1250};

int motor_status[STEPPING_MOTOR_SUM] = {-1};
int motor_foward[STEPPING_MOTOR_SUM] = {0};// true is 1, false is -1, stop is 0.
int motor_low_time[STEPPING_MOTOR_SUM] = {0};
volatile long pulse_count[STEPPING_MOTOR_SUM] = {0};


void motor_set_speed(int motor_no, boolean foward, int speed) {
  if (speed == 0) {
    motor_status[motor_no] = -1;
    motor_foward[motor_no] = 0;
    return ;
  } else {
    long range = (motor_max_low_time[motor_no] - motor_min_low_time[motor_no]);
    long low_time = motor_max_low_time[motor_no] - (range * speed /100);
    motor_low_time[motor_no] = low_time;
    if (foward == true) {
      motor_status[motor_no] = motor_cw[motor_no];
      motor_foward[motor_no] = 1;
    } else {
      motor_status[motor_no] = motor_ccw[motor_no];
      motor_foward[motor_no] = -1;
    }
    return;
  }
}

void motor_controll(int motor_no, int pulse_pin, bool *current_status, int *t, int low_time){
  if (*current_status == false){
      digitalWrite(pulse_pin, HIGH);
      *current_status = true;
      pulse_count[motor_no] += motor_foward[motor_no];
      *t = 4; // high_time. CMD2020P is good by this time.
    } else {
      digitalWrite(pulse_pin, LOW);
      *current_status = false;
      *t = low_time;
  }
}


void timer1_controll() {
  // time handler
  static int t[STEPPING_MOTOR_SUM] = {0};
  static bool is_high[STEPPING_MOTOR_SUM] = {false};
  int i = 0;
  for (i = 0; i < STEPPING_MOTOR_SUM; i++) {
    if (t[i] <= 0) {
      motor_controll(i, motor_status[i], &is_high[i], &t[i], motor_low_time[i]);
    }
      t[i]--;
  } 
}

void stepping_motor_init() {
  // stp motor init.p
  int i = 0;
  for (i = 0; i < STEPPING_MOTOR_SUM; i++){
    pinMode(motor_cw[i], OUTPUT);   
    pinMode(motor_ccw[i], OUTPUT);   
  }
}



void motorLiftCallback(const std_msgs::Int8& msg) {
  boolean tf = false;
  if (msg.data>=0){
    tf = true;
  }
  target_pulse = msg.data;
  motor_set_speed(2, tf, abs(msg.data));
}

ros::Subscriber<std_msgs::Int8> lift_sub("lift_req",motorLiftCallback);

void setup()
{
  pinMode(13, OUTPUT);
  nh.initNode();
  nh.subscribe(lift_sub);

  stepping_motor_init();
}

int count = 0;

void loop()
{
  if(target_pulse > 0){
    motor_set_speed(0,true,LIFT_PW);
    timer1_controll();
    target_pulse--;
  }else motor_set_speed(0,true,0);
  delayMicroseconds(1);
  nh.spinOnce();  
}

