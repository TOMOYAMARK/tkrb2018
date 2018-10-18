//ラインセンサーの値取得とその抽象化を行う
//そのうちクラスにしたい

#include <ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Int32.h>
#include <std_msgs/UInt8.h>

//##通常用途やつ##
std_msgs::UInt8 linesensorMessage;
ros::Publisher linesensorPublisher("linesensor", &linesensorMessage);

bool isLinesensorDebugMode = false;
//生の値とboolになった値を分けている
long linesensorRawValue[SENSOR_SUM] = {}; //ゼロ埋めされているハズ
bool linesensorValue[SENSOR_SUM] = {}; //trueが黒
static const int linesensorPinAssign[] = {4,5,3,6,2,7,1,8};
//インデックスは0が一番左
int linesensorCounter = 0;

void initLinesensor() {
    nh.advertise(linesensorPublisher);
    if(isLinesensorDebugMode) initDebug();
}

void linesensorMain() {
    updateLinesensorValue();
    //いつか分ける
    if(!(linesensorCounter > 500)) {
        //送信処理
        publishLinesensorValue();
        if(isLinesensorDebugMode)
            sendRawLinesensorValue();
    }
    linesensorCounter++;
}

//##ここから下の関数は使うな##
void publishLinesensorValue() {
    unsigned int message = 0;
    for(int i=0; i<SENSOR_SUM; i++)
        message += getLinesensorColor(i) << i;
    linesensorMessage.data = message;
    linesensorPublisher.publish(&linesensorMessage);
}

//配列をexternにするのが難しかった…
//boolにしたラインセンサの値
bool getLinesensorColor(int index) {
    if(index < SENSOR_SUM)
        return linesensorValue[index];
    //ほんとはfalseじゃなくてthrowしたい
    else return false;
}

void updateLinesensorValue() {
    for(int i=0; i<SENSOR_SUM; i++) {
        linesensorRawValue[i] = analogRead(linesensorPinAssign[i]);
        linesensorValue[i] = linesensorRawValue[i] < linesensorThreshold;
    }
}

//##デバッグ用各種変数関数##
std_msgs::Int32MultiArray debugLSArray; //さすがに長すぎるかなって
ros::Publisher linesensorDebugPublisher("DEBUG_LINESENSOR_VALUE", &debugLSArray);

void initDebug() {
    nh.advertise(linesensorDebugPublisher);
    debugLSArray.data_length = SENSOR_SUM;
    isLinesensorDebugMode = true;
}

//デバッグ用の送信やつ
void sendRawLinesensorValue() {
    debugLSArray.data = linesensorRawValue;
    linesensorDebugPublisher.publish(&debugLSArray);
}
