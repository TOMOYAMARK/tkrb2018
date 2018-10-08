#include "ros/ros.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Int16.h"
#include "std_msgs/Int32.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Joy.h"
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int8MultiArray.h"
#include "tkrb2018/PlanQueue.h"
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

int fieldMap[9][10] = {};//マッピング情報。各格子点の状態を要素とする配列。周囲１マス分を余分に確保して0で埋めている。
#define MAPW 9
#define MAPH 10
#define UNIT_SCALE 300//mm。１マスのおおきさ

#define PI 3.1416

/*------------------モータの情報--------------*/
#define STEP 1.8
#define WHEEL_DIAMETER 150//mm-?
#define MM_PER_PULSE (WHEEL_DIAMETER * PI / 360)*STEP//mm
#define DEFAULT_MOTOR_POW 90//max 100(%)
#define INNER_WHEEL_DISTANCE 380//mmタイヤ間距離

/*-------------------機体の情報-----------------*/
int selfPosition[2] = {4,9};//初期位置。下図のS
//機体の向いている方向。時計回り。
#define FORWARD 0
#define RIGHT 1
#define BACKWARD 2
#define LEFT 3
int headingDirection = FORWARD;//最初は前方を向いている。

enum stateParam {STOP = -1,IDLE = 0, WORKING = 1};
stateParam state = IDLE;//マシンのタスク受付状態
/*----------------------------------------------*/


struct leftRightStr{//足回り駆動用で使う左右の変数
  int l;
  int r;
};
struct leftRightStr targetPulse = {0,0};//目標パルス
struct leftRightStr motorPulseOutput = {0,0};//サブスクライブする変数
struct leftRightStr motorInput = {0,0};//最終的にmotor_input_l,rに代入する
std_msgs::Int8 motor_input_l,motor_input_r;//パブリッシュする変数


/*-------------------マップ情報-----------------*/
#define OBSTICLE -1
//ボールのある位置は、おいてあるボールの数で表記する。
//例）[3][4]にボールが２個->fieldMap[3][4] = 2;
/*---------------------------------------------*/


std_msgs::Int8 snapshotReq;//Publishする変数。
ros::Publisher webcamRequestPub;//ImageProcessing.pyに画像処理を依頼する。
ros::Publisher motorRInput,motorLInput;
ros::Publisher collectRequest;//回収用サーボを動かすリクエストを送る。
ros::Publisher liftRequest;//回収用ステピを動かすリクエストを送る。
ros::Subscriber webcamOutputSub;//ImageProcessing.pyから返る値を扱う。
ros::Subscriber testSub,controlerSub;//テスト用。
ros::Subscriber pulseLSub,pulseRSub;//足回りのパルス読み取り

ros::ServiceClient planQueueClient;
tkrb2018::PlanQueue planQueueSrv;


void initializeMap();//マップ関連情報の初期化。
void showMap();//マップ表示。デバッグ用
void snapshotCallback(const std_msgs::Int8MultiArray::ConstPtr& camResult);//CallBack
void testCallback(const std_msgs::String::ConstPtr& val);

/*--------------マップ上の情報更新---------------*/
void moveMachineOnMap(char mv);//下記の関数を用いてマシンを移動する。
void rotateMachine(int d);//回転
void forbackMachine(int d);//前進後退
/*---------------------------------------------*/


/*-----------------------コールバック関数------------*/
void takeSnapShot();
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);//テスト用
void pulseRCallback(const std_msgs::Int32::ConstPtr& pR);
void pulseLCallback(const std_msgs::Int32::ConstPtr& pL);
/*--------------------------------------------------*/


/*---------------走行系の関数-------------------*/
int cvtDegreesToPulse(double degs);//角度パラメータを入力パルスに変換
int cvtUnitToPulse(double units);//すすめるマスを入力パルスに変換
bool checkMoveProgress(char t,double par);//taskの動作が完了したかどうか、パルスを見て判断
void setTarget(char t, double par);//目標パルスを算出
bool checkTargetPulse(int val, int target, int ex = 0, bool cw = true);//目標パルスに入力パルスが近づいたら（ex(extention)=判定範囲）
void taskFlowHandler();//taskキューのメイン処理
/*---------------------------------------------*/

double startTime, endTime;//時間計測用変数


int main(int argc, char **argv)
{
  ros::init(argc, argv, "main");
  ros::NodeHandle n;
  snapshotReq.data = 1;//initVar
  initializeMap();
  showMap();
  webcamRequestPub = n.advertise<std_msgs::Int8>("snapshot_req", 1000);
  collectRequest = n.advertise<std_msgs::Int16>("collect_req", 1000);
  liftRequest = n.advertise<std_msgs::Int16>("lift_req", 1000);
  webcamOutputSub = n.subscribe("webcam_out", 1000, snapshotCallback);
  testSub = n.subscribe("test",1000,testCallback);
  ros::Subscriber joy = n.subscribe("joy",1000,joyCallback);
  pulseLSub = n.subscribe("pulse_l",1000,pulseLCallback);
  pulseRSub = n.subscribe("pulse_r",1000,pulseRCallback);
  motorLInput = n.advertise<std_msgs::Int8>("motor_l_input", 1000);
  motorRInput = n.advertise<std_msgs::Int8>("motor_r_input", 1000);

  planQueueClient = n.serviceClient<tkrb2018::PlanQueue>("plan_queue");

  ros::Rate loop_rate(10);

  while (ros::ok())
  {
    taskFlowHandler();
    ros::spinOnce();
    loop_rate.sleep();
  }


  return 0;
}

void pulseLCallback(const std_msgs::Int32::ConstPtr& pL) {
  motorPulseOutput.l = pL->data;
}
void pulseRCallback(const std_msgs::Int32::ConstPtr& pR) {
  motorPulseOutput.r = pR->data;
}

void initializeMap(){
  //フィールド中央のカゴを障害物とする。
  fieldMap[4][5] = OBSTICLE;//OBSTICLE = -1 マップ上は"X"
  //既知領域のボール位置
  fieldMap[4][7] = 1;
  fieldMap[5][6] = 1;
  fieldMap[6][8] = 1;
  fieldMap[2][5] = 1;
  fieldMap[2][7] = 2;
  /*- - - - - - - - -
    - 0 0 0 0 0 0 0 -
    - 0 0 0 0 0 0 0 -
    - 0 0 0 0 0 0 0 -
    - 0 0 0 0 0 0 0 -
    - 0 1 0 x 0 0 0 -
    - 0 0 0 0 1 0 0 -
    - 0 2 0 1 0 0 0 -
    - 0 0 0 0 0 1 0 -
    - - - - S - - - -
   */
}

void showMap(){
  ostringstream oss;
  string buf;
  string dir;
  for(int j=0;j<MAPH;j++){
    for(int i=0;i<MAPW;i++){
      int token = fieldMap[i][j];

      if(i == selfPosition[0] && j == selfPosition[1])oss << 'M';//マシンの自己位置'M'
      else if(token == -1)oss << 'x';//障害物'x'
      else if(token == 0)oss << '.';//その他の領域
      else oss << fieldMap[i][j];//ボールの位置
    }
    ROS_INFO((oss.str()).c_str());
    oss.str("");
    oss.clear(stringstream::goodbit);// ストリームの状態をクリアする。
  }
  ROS_INFO("\t\t selfPos(%d,%d)",selfPosition[0],selfPosition[1]);
  if(headingDirection == FORWARD)dir = "forward";
  else if(headingDirection == RIGHT)dir = "right";
  else if(headingDirection == LEFT)dir = "left";
  else if(headingDirection == BACKWARD)dir = "back";
  ROS_INFO("\t\t heading(%s)",dir.c_str());
  ROS_INFO("");
}

void moveMachineOnMap(char mv){
  if (mv == 'F' || mv == 'f')forbackMachine(FORWARD);
  else if(mv == 'B' || mv == 'b')forbackMachine(BACKWARD);
  else if(mv == 'R' || mv == 'r')rotateMachine(RIGHT);
  else if(mv == 'L' || mv == 'l')rotateMachine(LEFT);
}

void snapshotCallback(const std_msgs::Int8MultiArray::ConstPtr& camResult){
  //ImageProcessingからのデータ処理 camResult.data = {正面か側面か、○マス先、何が}
  int cameraHeading = camResult->data[0];//正面か側面か
  int distanceByUnits = camResult->data[1];//何マス先か
  int thing = camResult->data[2];//何が(-1(OBSTICLE),1(BALL),2(BALLS),3,...)
  
}

void rotateMachine(int d){// d=RIGHT or LEFT 左旋回、右旋回を反映
  if(d == RIGHT){
    headingDirection++;
    if(headingDirection == 4)headingDirection = 0;
  }else if(d == LEFT){
    headingDirection--;
    if(headingDirection == -1)headingDirection = 3;
  }
}

void forbackMachine(int d){// d=FORWARD or BACKWARD 前進、後退を反映
  if(d == FORWARD){
    switch(headingDirection){
    case FORWARD:
      selfPosition[1]--;
      break;
    case RIGHT:
      selfPosition[0]++;
      break;
    case BACKWARD:
      selfPosition[1]++;
      break;
    case LEFT:
      selfPosition[0]--;
      break;
    }
  }else if(d == BACKWARD){
    switch(headingDirection){
    case FORWARD:
      selfPosition[1]++;
      break;
    case RIGHT:
      selfPosition[0]--;
      break;
    case BACKWARD:
      selfPosition[1]--;
      break;
    case LEFT:
      selfPosition[0]++;
      break;
    }
  }
}

void takeSnapShot(){
  webcamRequestPub.publish(snapshotReq);
}


void testCallback(const std_msgs::String::ConstPtr& val){
  if(val->data == "f" || val->data == "F"){
    moveMachineOnMap('F');
  }else if(val->data == "b" || val->data == "B"){
    moveMachineOnMap('B');
  }else if(val->data == "r" || val->data == "R"){
    rotateMachine(RIGHT);
  }else if(val->data == "l" || val->data == "L"){
    rotateMachine(LEFT);
  }
  showMap();
}


int cvtUnitToPulse(double units){
  double additionalPulse = units * UNIT_SCALE / MM_PER_PULSE;
  //ROS_INFO("additionalPulse %f",additionalPulse);
  return additionalPulse;
}

int cvtDegreesToPulse(double degs){
  double distance = (PI * INNER_WHEEL_DISTANCE * (degs/360));
  double opdis = MM_PER_PULSE;  
  double result = distance / opdis;
  return result;
}

void setMotorSpeed(int l,int r){
  motorInput.l = l;
  motorInput.r = r;
}

void publishMotorInput(){
  motor_input_l.data = motorInput.l;
  motor_input_r.data = motorInput.r;
  motorLInput.publish(motor_input_l);
  motorRInput.publish(motor_input_r);
}

void setTarget(char t, double par){
  switch(t){
  case 'f':
    {
    //前進操作
    targetPulse = {motorPulseOutput.l + cvtUnitToPulse(par),
		   motorPulseOutput.r + cvtUnitToPulse(par)};
    setMotorSpeed(DEFAULT_MOTOR_POW,DEFAULT_MOTOR_POW);
    ROS_INFO("setTargetTo %d",targetPulse.l);
    state = WORKING;
    break;
    }
  case 'b':
    {
    //後退操作
    targetPulse = {motorPulseOutput.l - cvtUnitToPulse(par),
		   motorPulseOutput.r - cvtUnitToPulse(par)};
    setMotorSpeed(-DEFAULT_MOTOR_POW,-DEFAULT_MOTOR_POW);
    state = WORKING;
    break;
    }
  case 'r':
    {
    //右旋回
    targetPulse = {motorPulseOutput.l + cvtDegreesToPulse(par),
		   motorPulseOutput.r - cvtDegreesToPulse(par)};
    setMotorSpeed(DEFAULT_MOTOR_POW,-DEFAULT_MOTOR_POW);
    state = WORKING;
    break;
    }
  case 'l':
    {
    //左旋回
    targetPulse = {motorPulseOutput.l - cvtDegreesToPulse(par),
		   motorPulseOutput.r + cvtDegreesToPulse(par)};
    setMotorSpeed(-DEFAULT_MOTOR_POW,DEFAULT_MOTOR_POW);
    state = WORKING;
    break;
    }
  case 'p':
    {
    //一定時間動作停止
    startTime = ros::Time::now().toSec();//時間計測開始
    setMotorSpeed(0,0);
    state = WORKING;
    ROS_INFO("pause at:%f",startTime);
    break;
    }
  case 'c':
    {
    //回収部操作
    std_msgs::Int16 ang;
    ang.data = par;
    collectRequest.publish(ang);//0->close 180->open
    state = IDLE;//サーボを動かす間もタスクを受け付ける（止めたい場合は'p'を使う）
    break;
    }
  case 'z':
    {
    //回収部を持ち上げるステピの制御
    std_msgs::Int16 liftPulse;
    liftPulse.data = par;
    liftRequest.publish(liftPulse);// + -
    state = IDLE;//ステピを動かす間もタスクを受け付ける
    break;
    }
  default:
    return;
  }
  
}

bool checkTargetPulse(int val, int target, int ex, bool cw){
  if(cw == true){
    if(val > target-ex)return 1;
    else return 0;
  }else if(cw == false){
    if(val < target+ex)return 1;
    else return 0;
  }else ROS_INFO("FALSE INPUT");
}
     

bool checkMoveProgress(char t, double par){
  switch(t){
  case 'f':
    {
    //前進操作
    if(checkTargetPulse(motorPulseOutput.l,targetPulse.l,40,true) &&
       checkTargetPulse(motorPulseOutput.r,targetPulse.r,40,true))
      return 1;
    break;
    }
  case 'b':
    {
    //後退操作
    if(checkTargetPulse(motorPulseOutput.l,targetPulse.l,40,false) &&
       checkTargetPulse(motorPulseOutput.r,targetPulse.r,40,false))
      return 1;
    break;
    }
  case 'r':
    {
    //右旋回
    if(checkTargetPulse(motorPulseOutput.l,targetPulse.l,0,true) &&
       checkTargetPulse(motorPulseOutput.r,targetPulse.r,0,false))
      return 1;
    break;
    }
  case 'l':
    {
    //左旋回
    if(checkTargetPulse(motorPulseOutput.l,targetPulse.l,0,false) &&
       checkTargetPulse(motorPulseOutput.r,targetPulse.r,0,true))
      return 1;
    break;
    }
  case 'p':
    {
    //一時停止
    endTime = ros::Time::now().toSec();
    double timeCount = endTime - startTime;
    if(timeCount > par){
      return 1;
    }
    break;
    }
  default:
    break;
  }

  return 0;
}

void getNextTask(char& task, double& param){
  static string task_buf = "x";
  planQueueSrv.request.request = "Next";//次のタスクを取得
  if (planQueueClient.call(planQueueSrv)){
    task_buf = planQueueSrv.response.task;
    param = planQueueSrv.response.param;
    task = task_buf[0];
  }
  else{
    ROS_ERROR("Failed to call service");
  }
}

void taskFlowHandler(){
  static char task = 'x';
  static double param = 0.0;

  if(state == STOP)return;
  else if(state == IDLE){
    getNextTask(task,param);
    setTarget(task,param);
    ROS_INFO("accepted task");
    ROS_INFO("%c,%f",task,param);
  }else if(state == WORKING){
    if(checkMoveProgress(task,param) == true){//目標パルスに届いたら
      ROS_INFO("reached");
      setMotorSpeed(0,0);//マシンを止める。
      state = IDLE;//次の動作を受け付ける。
      moveMachineOnMap(task);
      showMap();
    }
  }
  publishMotorInput();
}

void joyCallback(const sensor_msgs::Joy::ConstPtr& joy){
  if(joy -> buttons[1]){
    //Controller ボタン２
    std_msgs::Int16 val;
    val.data = 180;
    collectRequest.publish(val);
  }else if(joy -> buttons[2]){
    //Controller ボタン３
  }else if(joy -> buttons[3]){
    //Controller ボタン４
  }else if(joy -> buttons[0]){
    //Controller ボタン１
    std_msgs::Int16 val;
    val.data = 0;
    collectRequest.publish(val);
  }
}
