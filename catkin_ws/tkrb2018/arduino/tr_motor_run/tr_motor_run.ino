/* 
 * rosserial::std_msgs::Float64 Test
 * Receives a Float64 input, subtracts 1.0, and publishes it
 */

#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int8.h>

#define STEPPING_MOTOR_SUM 3
#define WHEEL_DIAMETER 200 
#define STEP 1.8
#define DEFAULTPW 100
#define ONE_GRID_PULSE 2000 //XXX
#define PULSE_FOWARD (WHEEL_DIAMETER * 3.14 * STEP / 360)

ros::NodeHandle nh;

float x; 

// stepping motor
static const int motor_cw[STEPPING_MOTOR_SUM] = {35, 36, 46};//{10, 8, 6};
static const int motor_ccw[STEPPING_MOTOR_SUM] = {38, 34, 36};//{9, 7, 5};
//static const int motor_min_low_time[STEPPING_MOTOR_SUM] = {50, 50, 50};
//static const int motor_max_low_time[STEPPING_MOTOR_SUM] = {200, 200, 200};
static const int motor_min_low_time[STEPPING_MOTOR_SUM] = {200, 200, 1000};
static const int motor_max_low_time[STEPPING_MOTOR_SUM] = {1250, 1250, 1000};

/***
   system_arguments.
     Don't use these arg names. 
     To use timer interrupt.(cannot pass values directry?)
***/
int motor_status[STEPPING_MOTOR_SUM] = {-1, -1, -1};
int motor_foward[STEPPING_MOTOR_SUM] = {0, 0, 0};// true is 1, false is -1, stop is 0.
int motor_low_time[STEPPING_MOTOR_SUM] = {0, 0, 0};
volatile long pulse_count[STEPPING_MOTOR_SUM] = {0, 0, 0};


void motor_set_speed(int motor_no, boolean foward, int speed) {
  /***
      @param motor_no: int, motor number. 
      @param foward: bool, true is foward, false is back.
      @param speed: int, 0 ~ 100. 0 is stop.
  ***/
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
  static int t[STEPPING_MOTOR_SUM] = {0, 0};
  static bool is_high[STEPPING_MOTOR_SUM] = {false, false};
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


void motorLCB(const std_msgs::Int8& msg) {
  boolean tf = false;
  if (msg.data>=0){
    tf = true;
  }
  motor_set_speed(0, tf, abs(msg.data));
}

void motorRCB(const std_msgs::Int8& msg) {
  boolean tf = false;
  if (msg.data>=0){
    tf = true;
  }
  motor_set_speed(1, tf, abs(msg.data));
}

void motorLinearCB(const std_msgs::Int8& msg) {
  
    boolean tf = false;
  if (msg.data>=0){
    tf = true;
  }
  motor_set_speed(2, tf, abs(msg.data));
}

void messageCb( const std_msgs::Float64& msg){
  x = msg.data - 1.0;
  digitalWrite(13, HIGH-digitalRead(13));   // blink the led
}

std_msgs::Int32 pulse0;
std_msgs::Int32 pulse1;
ros::Subscriber<std_msgs::Int8> motorL_sub("motor_l_input",motorLCB);
ros::Subscriber<std_msgs::Int8> motorR_sub("motor_r_input",motorRCB);
ros::Publisher pulse0_pub ("pulse_l", &pulse0);
ros::Publisher pulse1_pub ("pulse_r", &pulse1);

void setup()
{
  pinMode(13, OUTPUT);
  nh.initNode();
  nh.subscribe(motorL_sub);
  nh.subscribe(motorR_sub);
  nh.advertise(pulse0_pub);
  nh.advertise(pulse1_pub);

  stepping_motor_init();
}

int count = 0;

void loop()
{
  timer1_controll();

  if(count > 10000){
    pulse0.data = pulse_count[0];
    pulse1.data = pulse_count[1];
    pulse0_pub.publish(&pulse0);
    pulse1_pub.publish(&pulse1);
    count = 0;
  }
  delayMicroseconds(1);
  count++;
  nh.spinOnce();  
}

