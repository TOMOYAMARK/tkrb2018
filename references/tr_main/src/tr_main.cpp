#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Int16.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Char.h"
//#include "tr_main/PlanCommand.h"
//#include "tr_main/PlanParam.h"
//#include "tr_controller/motor.h"
#include <sstream>
#define TUNE_PULSE 45

#define THRESHOLD 0.7
#define DEFAULTPW 90
#define WHEEL_DIAMETER 87.5 * 2   //mm
#define INNER_WHEEL_DISTANCE 585  //mm
#define PI 3.14 //pi
#define PULSE_PER_STEP 1.8 //degree
#define ONE_PULSE_DISTANCE WHEEL_DIAMETER * PI * PULSE_PER_STEP / 360 //mm
#define ONE_GRID_DISTANCE 280//300 //mm
#define LINE_TRACE_GAIN 20//15 //xxx
#define ST_UP 800//extra time for waiting lift-up
#define ST_ARMS 20//extra time for waiting arms to close/open
#define TUNE 0.6 //(masu)

//ライントレース用に追加////
#define ON_LINE_DELAY 8//1//横ライン上に乗る時間
//#define ON_LINE_LIMIT 1000//エラー対策の上限
#define ON_LINE 0
#define ON_LINE_TUNE 33//
#define ONLY_L 1
#define ONLY_R 2
#define ELSE 4
//#define READY_TO_TURN 5 //目的のライン上に乗った瞬間にこのモードに

int ready_to_turn = 0; //1-->曲がる準備に入っている
///////////////////////////

std_msgs::Int8 power_default, power_motor_l, power_motor_r;
std_msgs::Int8 servo_action;
int pulse_motor_r, pulse_motor_l;
int line_gray[8] = {790,757,802, 906, 734, 773,700,700};

//ライントレース用に追加//////
int line_sensor_value[8];
int cnt = 0;
int line_count = 0;
int trace_of_trace = ELSE;// ON_LINE , ONLY_L , ONLY_R , ELSE , READY_TO_TURN
int count_line = 0; //fのparamが整数なら1->ラインカウント用処理へ, 小数なら0->従来のf処理へ
//////////////////////////////

ros::ServiceClient plan_command_client;
ros::ServiceClient plan_param_client;

//const char plan[100] = {'f', 3, 'r', 90,'l', 90,'b', 3,'u', 0, 's', 0};
//const char plan[100] = {'c', 1, 's', 10, 'f', 1}

const char plan[100] = {'c', 's', 'e', 'c', 's', 'l', 'b', 'c', 's', 'f', 'c', 's', 'r', 'b', 'c', 's', 'f', 'c', 's', 'r', 'b', 'c', 's', 'f', 'c', 's', 'r', 'b', 'c', 's', 'f', 'c', 's', 'r', 'f', 'r', 'u', 's', 'f', 'p', 's', 'f', 'b', 'f', 'b', 'f', 'b', 'b', 'f', 'b', 'z', 's', 'b'};
const double plan_param[100] = {1, ST_ARMS, 1.3, 2, ST_ARMS*2, 90, TUNE, 1, ST_ARMS, 2 + TUNE, 2, ST_ARMS*2, 90, TUNE, 1, ST_ARMS, 6 + TUNE, 2, ST_ARMS*2, 90, TUNE, 1, ST_ARMS, 4 + TUNE, 2, ST_ARMS*2, 90, TUNE, 1, ST_ARMS, 4 + TUNE, 2, ST_ARMS*2, 90, 2, 90, 1, ST_UP, 1, 2, 600, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0, 50, 3};


//const char        plan[100] = {'f','s'};
//const double plan_param[100]= { 5, 100};

//====== yosen plan======

//const char        plan[100] = {'c', 's', 'f', 'c', 's', 'u','c', 's', 'f', 'p', 's', 'f', 'b', 'f', 'b', 'f', 'b', 'f', 'b', 'f', 'b', 'z', 's', 'b'};
//const double plan_param[100]= { 1 , 10 , 1.3 , 2,  80  ,2,  3, 750, 2,  2,   300, 0.1, 0.1,0.1, 0.1,0.1, 0.1,0.1, 0.1,0.1, 0.1, 0, 50, 3};


//===== test =====
//const char        plan[100] = {'r', 's','l', 's', 'f', 'b', 'r', 's', 'l'};
//const double plan_param[100]= {90, 5, 90, 10, 0.1};



/*****
f: foward: grid_sum. watch line
e: foward: grid_sum  no watch line
b: back: grid_sum. 
l: left: degree
r: right: degree 
s: stop: 0.x sec (only integer)
c: collect: servo collect function.: 1 is just opening hands, 2 is async. 3 is close.
u: up the linia.: 1 is sync, 2 is async. xxxonly async works. 
p: pull nuno.: : 1 is sync, 2 is async. xxxonly async works.
a: prepare
z: reset nuno.
 *****/

ros::Publisher servo_pub; 


int state = -1; //0 is to set, 1 is working, -1 is waiting.
int number = 0;

void lineSensor0Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[0] = msg->data;
}
void lineSensor1Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[1] = msg->data;
}
void lineSensor2Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[2] = msg->data;
}
void lineSensor3Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[3] = 0;
}
void lineSensor4Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[4] = msg->data;
  //line_sensor_value[4] = 1000;
}
void lineSensor5Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[5] = msg->data;
}
void lineSensor6Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[6] = msg->data;
}
void lineSensor7Callback(const std_msgs::Int16::ConstPtr& msg) {
  line_sensor_value[7] = msg->data;
}

void pulseMotorRCallback(const std_msgs::Int32::ConstPtr& msg) {
  pulse_motor_r = msg->data;
}
void pulseMotorLCallback(const std_msgs::Int32::ConstPtr& msg) {
  pulse_motor_l = msg->data;
}

ros::Publisher command_pub;

const int down_linia = 1;
const int stop_linia = 2;
const int down_nuno = 3;
const int stop_nuno = 4;
const int up = 5;
const int pull_nuno = 6;
const int setup = 7;
const int reset_nuno = 8;
std_msgs::Int8 command;

void keyCallback(const std_msgs::Char::ConstPtr& msg) {
  char key = msg->data;
  int LineGray(int a,int b);
  static int white[8];
  static int black[8];
  int line_num = 0;
  char line_set = key;

  int LineGray(int a,int b);
  
  if (key == 'l') {
    command.data = down_linia;
    command_pub.publish(command);
  } else if (key == 's') {
    command.data = stop_linia;
    command_pub.publish(command);    
    command.data = stop_nuno;
    command_pub.publish(command);    
  } else if (key == 'n') {
    command.data = down_nuno;
    command_pub.publish(command);
  } else if (key == 'u') {
    command.data = up;
    command_pub.publish(command);
  } else if (key == 'p') {
    command.data = pull_nuno;
    command_pub.publish(command);
  } else if (key == 'r') {
    //reset
    state = -1;
    number = 0;
  } else if (key == 'g') {
    state = 0;
    number = 0;
    power_motor_l.data = 0;
    power_motor_r.data = 0;
  } else if (key == 'w'){//white
    white[0] = line_sensor_value[0];
    white[1] = line_sensor_value[1];
    white[2] = line_sensor_value[2];
    white[3] = line_sensor_value[3];//get the data of line sensor when white
    white[4] = line_sensor_value[4];
    white[5] = line_sensor_value[5];
    white[6] = line_sensor_value[6];
    white[7] = line_sensor_value[7];
    ROS_INFO("sensor0-white:%d",white[0]);
    ROS_INFO("sensor1-white:%d",white[1]);
    ROS_INFO("sensor2-white:%d",white[2]);
    ROS_INFO("sensor3-white:%d",white[3]);
    ROS_INFO("sensor4-white:%d",white[4]);
    ROS_INFO("sensor5-white:%d",white[5]);
    ROS_INFO("sensor6-white:%d",white[6]);
    ROS_INFO("sensor7-white:%d",white[7]);
  } else if (key == 'k'){//kuro
    black[0] = line_sensor_value[0];
    black[1] = line_sensor_value[1];
    black[2] = line_sensor_value[2];
    black[3] = line_sensor_value[3];//get the data of line sensor when black
    black[4] = line_sensor_value[4];
    black[5] = line_sensor_value[5];
    black[6] = line_sensor_value[6];
    black[7] = line_sensor_value[7];
    ROS_INFO("sensor0-black:%d",black[0]);
    ROS_INFO("sensor1-black:%d",black[1]);
    ROS_INFO("sensor2-black:%d",black[2]);
    ROS_INFO("sensor3-black:%d",black[3]);
    ROS_INFO("sensor4-black:%d",black[4]);
    ROS_INFO("sensor5-black:%d",black[5]);
    ROS_INFO("sensor6-black:%d",black[6]);
    ROS_INFO("sensor7-black:%d",black[7]);
  } else if (key == 'a') { // setup default
    command.data = setup;
    command_pub.publish(command);    
  }
  

  else if (key == 'x') {  // calculate
  line_gray[0] = LineGray(white[0],black[0]);
  line_gray[1] = LineGray(white[1],black[1]);
  line_gray[2] = LineGray(white[2],black[2]);
  line_gray[3] = LineGray(white[3],black[3]);//get the threshold of line sensor
  line_gray[4] = LineGray(white[4],black[4]);
  line_gray[5] = LineGray(white[5],black[5]);
  line_gray[6] = LineGray(white[6],black[6]);
  line_gray[7] = LineGray(white[7],black[7]);
  //
  ROS_INFO("sensor0:%d",line_gray[0]);
  ROS_INFO("sensor1:%d",line_gray[1]);
  ROS_INFO("sensor2:%d",line_gray[2]);
  ROS_INFO("sensor3:%d",line_gray[3]);
  ROS_INFO("sensor4:%d",line_gray[4]);
  ROS_INFO("sensor5:%d",line_gray[5]);
  ROS_INFO("sensor6:%d",line_gray[6]);
  ROS_INFO("sensor7:%d",line_gray[7]);
  }
}
int LineGray(int a,int b) {
   return a/4 + b*3/4;
}// white/4 + black *3/4 
  
double turnPulse(double degree) {
  // add the result to one param and sub the result to another param
  double distance = (PI * INNER_WHEEL_DISTANCE * degree/360);
  double opdis = ONE_PULSE_DISTANCE;  
  double result = distance / opdis;
  //  ROS_INFO("distance: %f", distance);
  //  ROS_INFO("op_distance: %f", opdis);  
  //  ROS_INFO("result: %f", result);
  return result * (88.0/100.0);
}

double fowardPulse(double grid_sum) {
  double tmp = (ONE_GRID_DISTANCE * grid_sum);
  double tmp2 = ONE_PULSE_DISTANCE;
  double tmp3 = tmp/tmp2;
  //  ROS_INFO("grid_sum: %f", grid_sum);
  //  ROS_INFO("foward_pulse: %f", tmp);
  //  ROS_INFO("foward_pulse2: %f", tmp2);
  //  ROS_INFO("foward_pulse3: %f", tmp3);
  //return (double)((ONE_GRID_DISTANCE * grid_sum) / ONE_PULSE_DISTANCE);
  return (double)tmp3;
}

int count_sensor_black(){
  int blcnt = 0; 
  for(int i = 0;i<8;i++)
    if(line_sensor_value[i] > line_gray[i])blcnt++;
  return blcnt;
}

void line_trace(int default_pw) {
  //ROS_INFO("line_sensor_value: %d, %d, %d, %d, gray: %d", line_sensor_value[0], line_sensor_value[1],line_sensor_value[2],line_sensor_value[3], line_gray[0]);


  power_motor_l.data = default_pw;//- default_pw / LINE_TRACE_GAIN;
  power_motor_r.data = default_pw;//- default_pw / LINE_TRACE_GAIN;

    
  if (count_sensor_black() >= 4 /*&& line_sensor_value[3] > line_gray[3] && line_sensor_value[2] > line_gray[2]*/){ // cross a line
      // pass
      //if(trace_of_trace != ON_LINE){
      //line_count++;
      //ROS_INFO("!!!!!!!%d!!!!!!!!!!!",line_count);
      //}
      trace_of_trace = ON_LINE;
      ROS_INFO("ONLINE");
  }
  else if(line_sensor_value[3] > line_gray[3] && line_sensor_value[4] < line_gray[4]){//->TURN LEFT
    power_motor_l.data = default_pw -(LINE_TRACE_GAIN * 1/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_r.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;
    trace_of_trace = ELSE;

    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  }
  else if (line_sensor_value[3] < line_gray[3] && line_sensor_value[4] > line_gray[4]){//->TURN RIGHT
    power_motor_l.data = default_pw +LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;
    power_motor_r.data = default_pw - (LINE_TRACE_GAIN * 1/10);//- default_pw / LINE_TRACE_GAIN;
    trace_of_trace = ELSE;

    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  }
  else if (line_sensor_value[2] > line_gray[2]){  // turn left.
    power_motor_l.data = default_pw - (LINE_TRACE_GAIN * 2/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_r.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;    
    trace_of_trace = ONLY_L;
    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  } else if (line_sensor_value[5] > line_gray[5]){  // turn right
    power_motor_r.data = default_pw - (LINE_TRACE_GAIN * 2/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_l.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;    
    trace_of_trace = ONLY_R;
    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  }
  else if (line_sensor_value[1] > line_gray[1]){  // turn left.
    power_motor_l.data = default_pw -(LINE_TRACE_GAIN * 6/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_r.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;    
    trace_of_trace = ONLY_L;
    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  } else if (line_sensor_value[6] > line_gray[6]){  // turn right
    power_motor_r.data = default_pw -(LINE_TRACE_GAIN * 6/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_l.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;    
    trace_of_trace = ONLY_R;
    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  }
    else if (line_sensor_value[0] > line_gray[0]){  // turn left.
    power_motor_l.data = default_pw -(LINE_TRACE_GAIN * 8/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_r.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;    
    trace_of_trace = ONLY_L;
    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  } else if (line_sensor_value[7] > line_gray[7]){  // turn right
    power_motor_r.data = default_pw -(LINE_TRACE_GAIN * 8/10);//- default_pw / LINE_TRACE_GAIN;
    power_motor_l.data = default_pw + LINE_TRACE_GAIN;//- default_pw / LINE_TRACE_GAIN;    
    trace_of_trace = ONLY_R;
    ROS_INFO("oooooooooooooooooooooo  %d  ooooooooooooooo",power_motor_l.data - power_motor_r.data);
  }
  else trace_of_trace = ELSE;
    
}

/*
int state = 0; //0 is to set, 1 is working.
int number = 0;
char task = 'x';
double param = 0.0;
int target_r = 0;
int target_l = 0;  
*/


void move_handler() {
  
  static char task = 'x';
  static double param = 0.0;
  static int target_r = 0;
  static int target_l = 0;
  static int count = 0; // time count
  if (state == -1) {
    return;
  } else if (state == 0) {
    tr_main::PlanCommand command_srv;
    tr_main::PlanParam param_srv;    
    command_srv.request.i = number;
    param_srv.request.i = number;    
    plan_command_client.call(command_srv);
    plan_param_client.call(param_srv);
    task = command_srv.response.result.c_str()[0];
    param = (double)param_srv.response.result;
    
    //task = plan[number];
    //param = (double)plan_param[number];

    
    ROS_INFO("move: %c", task);
    ROS_INFO("param: %f", param);    
    switch(task) {
      //********** set target **********//
    case 'f':
      target_r = pulse_motor_r + fowardPulse(param);//one pulse * param
      target_l = pulse_motor_l + fowardPulse(param);
      //      ROS_INFO("param:%f", param);
      //      ROS_INFO("fwp:%f", fowardPulse(param));
      //      ROS_INFO("pmr:%d", pulse_motor_r);
      //      ROS_INFO("tr:%d", target_r);      
      power_motor_r.data = DEFAULTPW;
      power_motor_l.data = DEFAULTPW;
      state = 1;      
      break;
    case 'd':
      target_r = pulse_motor_r + fowardPulse(param);//one pulse * param
      target_l = pulse_motor_l + fowardPulse(param);
      //      ROS_INFO("param:%f", param);
      //      ROS_INFO("fwp:%f", fowardPulse(param));
      //      ROS_INFO("pmr:%d", pulse_motor_r);
      //      ROS_INFO("tr:%d", target_r);      
      power_motor_r.data = DEFAULTPW;
      power_motor_l.data = DEFAULTPW;
      state = 1;      
      break;
    case 'e':
      target_r = pulse_motor_r + fowardPulse(param);//one pulse * param
      target_l = pulse_motor_l + fowardPulse(param);    
      power_motor_r.data = DEFAULTPW;
      power_motor_l.data = DEFAULTPW;
      state = 1;
      break;
    case 'b':
      target_r = pulse_motor_r - fowardPulse(param);
      target_l = pulse_motor_l - fowardPulse(param);
      power_motor_r.data = -DEFAULTPW;
      power_motor_l.data = -DEFAULTPW;
      state = 1;      
      break;
    case 'r':
      target_r = pulse_motor_r - turnPulse(param);
      target_l = pulse_motor_l + turnPulse(param);
      power_motor_r.data = -DEFAULTPW;
      power_motor_l.data = DEFAULTPW;
      state = 1;      
      break;
    case 'l':
      target_r = pulse_motor_r + turnPulse(param);
      target_l = pulse_motor_l - turnPulse(param);
      power_motor_r.data = DEFAULTPW;
      power_motor_l.data = -DEFAULTPW;
      state = 1;      
      break;      
    case 's'://stop
      power_motor_r.data = 0;
      power_motor_l.data = 0;
      count = param;
      state = 1;      
      break;
    case 'c'://collect
      servo_action.data = param;
      servo_pub.publish(servo_action);
      state = 0;
      break;
    case 'u'://up
      command.data = up;
      command_pub.publish(command);
      break;
    case 'p'://pull nuno
      command.data = pull_nuno;
      command_pub.publish(command);
      break;
    case 'z'://reset nuno
      command.data = reset_nuno;
      command_pub.publish(command);
      break;
    default:
      break;
    }    
    number += 1;
  } else if (state == 1) {
    
    //********** finish check **********//
    //    ROS_INFO("tr:%d, pr:%d, tl:%d, pl:%d", target_r, pulse_motor_r, target_l, pulse_motor_l);
    switch(task) {      
    case 'f':
      if(ready_to_turn == 1 && target_r < pulse_motor_r && target_l < pulse_motor_l){//軸位置調整に時間でなくモーターパルスを利用する場合
	ready_to_turn = 0;
	power_motor_l.data = 0;
	power_motor_r.data = 0;
	state = 0;
	cnt = 0;
      }
      else if(target_r-TUNE_PULSE < pulse_motor_r && target_l-TUNE_PULSE < pulse_motor_l && ready_to_turn == 0){
	ROS_INFO("checking.....");
	line_trace(DEFAULTPW);
	if(trace_of_trace == ON_LINE){
	  ready_to_turn = 1;
	  ROS_INFO("ON_TARGET_LINE!");
	  
	  target_r = ON_LINE_TUNE + pulse_motor_r;
	  target_l = ON_LINE_TUNE + pulse_motor_l;
	}
	
      } else if (target_r + TUNE_PULSE < pulse_motor_r &&  target_l + TUNE_PULSE < pulse_motor_l){
	power_motor_l.data = 0;
	power_motor_r.data = 0;
	state = 0;
	cnt = 0;
	ready_to_turn = 0;
      }
      
      
      
      /* if(target_r-TUNE_PULSE < pulse_motor_r && target_l-TUNE_PULSE < pulse_motor_l){//軸位置調整に呼び出し回数（時間）を利用する場合(DEFAULTPW = 60　依存)
	ROS_INFO("checking.....");
	line_trace(DEFAULTPW);
	if(trace_of_trace == ON_LINE && ready_to_turn == 0){
	  ready_to_turn = 1;
	  ROS_INFO("ON_TARGET_LINE!");
	  ROS_INFO("--------------%d--to--%d---------",cnt,ON_LINE_DELAY);
	  
	}else if(ready_to_turn == 1){//目的のラインに乗ったあとの動作　
    	  cnt++;
	  ROS_INFO("--------------%d--to--%d---------",cnt,ON_LINE_DELAY);
	  if(cnt >= ON_LINE_DELAY){ //一定時間横ライン上に乗ったら次の動作へ
	    powr_motor_l.data = 0;
	    power_motor_r.data = 0;
	    state = 0;
	    cnt = 0;
	    ready_to_turn = 0;//初期化処理
	  }
	}
	
      } else if (target_r + TUNE_PULSE < pulse_motor_r &&  target_l + TUNE_PULSE < pulse_motor_l){
	power_motor_l.data = 0;
	power_motor_r.data = 0;
	state = 0;
	cnt = 0;
	ready_to_turn = 0;
      }
      */
      
  else {
	line_trace(DEFAULTPW);
      }
      break;
    case 'e':
      if(target_r < pulse_motor_r && target_l < pulse_motor_l){
	state = 0;
      }else{
	line_trace(DEFAULTPW);
      }
      break;
    case 'd'://４直線目の特殊処理
      if(target_r-TUNE_PULSE < pulse_motor_r && target_l-TUNE_PULSE < pulse_motor_l){
	ROS_INFO("checking.....");
	line_trace(DEFAULTPW);
	if(trace_of_trace == ON_LINE){
	  cnt++;
	  ROS_INFO("--------------%d--to--%d---------",cnt,ON_LINE_DELAY);
	  if(cnt >= ON_LINE_DELAY){ //一定時間横ライン上に乗ったら次の動作へ
	    power_motor_l.data = 0;
	    power_motor_r.data = 0;
	    state = 0;
	    cnt = 0;       //初期化処理
	    //line_count = 0;//
	    //count_line = 0;//
	  }
	}
      } else if (target_r + TUNE_PULSE < pulse_motor_r &&  target_l + TUNE_PULSE < pulse_motor_l){
	power_motor_l.data = 0;
	power_motor_r.data = 0;
	state = 0;
	cnt = 0; 	  
      }else {
	line_trace(DEFAULTPW);
      }
      break;
    case 'b':
      if (target_r > pulse_motor_r && target_l > pulse_motor_l) {
	state = 0;
      }
      break;
    case 'r':
      if (target_r > pulse_motor_r && target_l < pulse_motor_l) {
	state = 0;
      }
      break;
    case 'l':
      if (target_r < pulse_motor_r && target_l > pulse_motor_l) {
	state = 0;
     }
      break;
    case 's':
      count --;
      if (count < 0) {
	state = 0;
      }
      break;      
    }    
    
  }
}
  

//std_msg::Int8 mv; // 0静止　1前進　2後退　3左旋回　4右旋回


int main(int argc, char **argv)
{
  ros::init(argc, argv, "tr_main");
  ros::NodeHandle n;
  plan_command_client = n.serviceClient<tr_main::PlanCommand>("plan_command");
  plan_param_client = n.serviceClient<tr_main::PlanParam>("plan_param");    
  command_pub = n.advertise<std_msgs::Int8> ("sub_motor_command",1000);
  //ros::Subscriber joy = n.subscribe("joy",1000,joyCallback);
  //ros::Publisher motorR_pub = n.advertise<std_msgs::Int8>("motorR", 1000);
  //ros::Publisher motorL_pub = n.advertise<std_msgs::Int8>("motorL",1000);
  //ros::Publisher motor_pub = n.advertise<std_msgs::Int8MultiArray>("motor", 1000);
  //ros::Publisher motor_pub = n.advertise<tr_controller::motor>("motor",1000);
  //ros::Publisher move_pub = n.advertise<std_msgs::Int8>("move",1000);
  ros::Publisher motorL_pub = n.advertise<std_msgs::Int8>("power_motor_l",1000);
  ros::Publisher motorR_pub = n.advertise<std_msgs::Int8>("power_motor_r",1000);
  servo_pub = n.advertise<std_msgs::Int8>("servo_action",1000);
  ros::Subscriber pulse_motor_r_sub = n.subscribe("pulse_motor_r",1000, pulseMotorRCallback);
  ros::Subscriber pulse_motor_l_sub = n.subscribe("pulse_motor_l",1000, pulseMotorLCallback);
  ros::Subscriber line_sensor_0_sub = n.subscribe("chatter0",1000, lineSensor0Callback);
  ros::Subscriber line_sensor_1_sub = n.subscribe("chatter1",1000, lineSensor1Callback);
  ros::Subscriber line_sensor_2_sub = n.subscribe("chatter2",1000, lineSensor2Callback);
  ros::Subscriber line_sensor_3_sub = n.subscribe("chatter3",1000, lineSensor3Callback);
  ros::Subscriber line_sensor_4_sub = n.subscribe("chatter4",1000, lineSensor4Callback);
  ros::Subscriber line_sensor_5_sub = n.subscribe("chatter5",1000, lineSensor5Callback);
  ros::Subscriber key_sub = n.subscribe("tr_key",1000, keyCallback);    
  
  ros::Rate loop_rate(10);

  int count = 0;
  while (ros::ok())
  {
    move_handler();
    motorL_pub.publish(power_motor_l);
    motorR_pub.publish(power_motor_r);
    ros::spinOnce();
    loop_rate.sleep();
    ++count;
  }


  return 0;
}
