//モーター制御はクラス作ってライブラリにしたい…
//既製品あれば使いたい…

#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Int32.h>

//ラインセンサに使われているフォトセンサの数
#define SENSOR_SUM 8

#define STEPPING_MOTOR_SUM 2
#define WHEEL_DIAMETER 200
#define STEP 1.8
#define DEFAULTPW 100
#define ONE_GRID_PULSE 2000
#define PULSE_FOWARD (WHEEL_DIAMETER * 3.1416 * STEP / 360)

ros::NodeHandle nh;

// ラインセンサ用
static const int linesensorThreshold = 100;

// stepping motor 0が左で1が右だぞっ☆
static const int motor_cw[STEPPING_MOTOR_SUM] = {36, 52};
static const int motor_ccw[STEPPING_MOTOR_SUM] = {40, 48};
static const int motor_min_low_time[STEPPING_MOTOR_SUM] = {350, 350};//最初200, 最適100
static const int motor_max_low_time[STEPPING_MOTOR_SUM] = {1250, 1250};
static const float curveMotorGain[2] = {0.5, 0.25}; //0がゆるくカーブ、1がきつくカーブ

//ステピ用の怪しいやつ モーターに突っ込まれた値を使ってライントレースでほげほげする
struct { boolean forward; int speed; } motorVectorArray[STEPPING_MOTOR_SUM];
/***
     system_arguments.
     Don't use these arg names.
     To use timer interrupt.(cannot pass values directry?)
***/
// 
int motor_status[STEPPING_MOTOR_SUM] = {-1, -1};
int motor_foward[STEPPING_MOTOR_SUM] = {0, 0};// true is 1, false is -1, stop is 0.
int motor_low_time[STEPPING_MOTOR_SUM] = {0, 0};
volatile long pulse_count[STEPPING_MOTOR_SUM] = {0, 0};

int XcheckDelay = -1;//交差チェック用ディレイ
//-1が通常、0がチェック中、それ以外がディレイ中
//enumにしたい

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
        long low_time = motor_max_low_time[motor_no] - (range * speed / 100);
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

void motor_control(int motor_no, int pulse_pin, bool *current_status, int *t, int low_time) {
    if (*current_status == false) {
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

void timer1_control() {
    // time handler
    static int t[STEPPING_MOTOR_SUM] = {0, 0};
    static bool is_high[STEPPING_MOTOR_SUM] = {false, false};
    int i = 0;
    for (i = 0; i < STEPPING_MOTOR_SUM; i++) {
        if (t[i] <= 0) {
            motor_control(i, motor_status[i], &is_high[i], &t[i], motor_low_time[i]);
        }
        t[i]--;
    }
}

void stepping_motor_init() {
    // stp motor init.p
    int i = 0;
    for (i = 0; i < STEPPING_MOTOR_SUM; i++) {
        pinMode(motor_cw[i], OUTPUT);
        pinMode(motor_ccw[i], OUTPUT);
    }
}

void motorLCB(const std_msgs::Int8& msg) {
    boolean tf = false;
    if (msg.data >= 0) {
        tf = true;
    }
    int speed = abs(msg.data);
    motorVectorArray[0].forward = tf;
    motorVectorArray[0].speed = speed;
    motor_set_speed(0, motorVectorArray[0].forward, motorVectorArray[0].speed);
}

void motorRCB(const std_msgs::Int8& msg) {
    boolean tf = false;
    if (msg.data >= 0) {
        tf = true;
    }
    int speed = abs(msg.data);
    motorVectorArray[1].forward = tf;
    motorVectorArray[1].speed = speed;
    motor_set_speed(1, motorVectorArray[1].forward, motorVectorArray[1].speed);
}

void startXcheck(const std_msgs::Int8& msg) {
    XcheckDelay = msg.data;
}


std_msgs::Int32 pulse0;
std_msgs::Int32 pulse1;
ros::Subscriber<std_msgs::Int8> motorL_sub("motor_l_input", motorLCB);
ros::Subscriber<std_msgs::Int8> motorR_sub("motor_r_input", motorRCB);
ros::Publisher pulse0_pub ("pulse_l", &pulse0);
ros::Publisher pulse1_pub ("pulse_r", &pulse1);

std_msgs::Int8 dummyState;
ros::Subscriber<std_msgs::Int8> XCheckReqReciever("xcheck_req", startXcheck);
ros::Publisher XStopPublisher("xstop_req", &dummyState);

int count = 0;

void linetrace() {
    float motorGain[STEPPING_MOTOR_SUM] = {1, 1}; //通常は入力の100%で動作する
    if (getLinesensorColor(3) && getLinesensorColor(4)) {
        //do nothing
    } else if (isAnyOnL() && isAnyOnR()) {
        //do nothing
    } else if (!getLinesensorColor(3) && getLinesensorColor(4)) {
        motorGain[1] = curveMotorGain[0];
    } else if (getLinesensorColor(3) && !getLinesensorColor(4)) {
        motorGain[0] = curveMotorGain[0];
    } else if (isAnyOnL()) {
        motorGain[0] = curveMotorGain[1];
    } else if (isAnyOnR()) {
        motorGain[1] = curveMotorGain[1];
    }
    for(int i=0; i<STEPPING_MOTOR_SUM; i++)
        motor_set_speed(i, motorVectorArray[i].forward,
            motorGain[i]*((float)motorVectorArray[i].speed));
}

bool isAnyOnL() {
    return (getLinesensorColor(1) || getLinesensorColor(2));
}

bool isAnyOnR() {
    return (getLinesensorColor(5) || getLinesensorColor(6));
}

void Xcheck() {
    int XcheckSum = 0;
    for(int i = 0;i < SENSOR_SUM;i++){
      //sum up blacks
      //if on Cross
      XcheckSum += getLinesensorColor(i);
    }
    if(XcheckSum >= 3) {
        XStopPublisher.publish(&dummyState);
        XcheckDelay = -1;
    }
}

void setup()
{
    pinMode(13, OUTPUT);
    nh.initNode();
    nh.subscribe(motorL_sub);
    nh.subscribe(motorR_sub);
    nh.advertise(pulse0_pub);
    nh.advertise(pulse1_pub);

    stepping_motor_init();
    initLinesensor();
    initDebug();

    dummyState.data = 0;
    nh.subscribe(XCheckReqReciever);
    nh.advertise(XStopPublisher);
}

void loop()
{
    timer1_control();

    if (count > 10000) {
        linesensorMain();
        linetrace();
        pulse0.data = pulse_count[0];
        pulse1.data = pulse_count[1];
        pulse0_pub.publish(&pulse0);
        pulse1_pub.publish(&pulse1);

        if(XcheckDelay > 0) XcheckDelay --;

        if(XcheckDelay == 0) Xcheck();

        count = 0;
    }
    

    count++;
    nh.spinOnce();

    delayMicroseconds(1);
}


