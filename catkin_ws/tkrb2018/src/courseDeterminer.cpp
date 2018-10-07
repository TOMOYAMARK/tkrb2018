#include "courseDeterminer.h"
#include "ros/ros.h"
#include "std_msgs/int8.h"
#include <sstream>

//進路決定関数 外部で使うのはこれだけ
RoboCourse GetCourse() {
    int lineSensorValue;
    if(isOnCourse(lineSensorValue)) {
        ROS_INFO("Robo will go straight.");
        return RoboCourse::straight;
    }
    else if(isOnLeft(lineSensorValue)) {
        ROS_INFO("Robo will turn right.");
        return RoboCourse::right;
    }
    else if(inOnRight(lineSensorValue)) {
        ROS_INFO("Robo will turn left.");
        return RoboCourse::left;
    }
    else if(isOnCross(lineSensorValue)) {
        ROS_INFO("Robo is on the cross.");
        return RoboCourse::cross;
    }
    else if(isOnTCross(lineSensorValue)) {
        ROS_INFO("Robo is on the T-cross.");
        return RoboCourse::cross;
    }
    else if(isOnRCorner(lineSensorValue)) {
        ROS_INFO("Robo will turn right: Robo is on the corner.");
        return RoboCourse::rigit;
    }
    else if(isOnLCorner(lineSensorValue)) {
        ROS_INFO("Robo will turn left: Robo is on the corner.");
        return RoboCourse::left;
    }
}

//現状把握関数
bool isOnCourse(const int lsValue) {
    if((lsValue & 0b00010000) || (lsValue & 0b00001000))
        return true;
    else return false;
}

bool isOnLeft(const int lsValue) {
    if(lsValue & 0b00000111)
        return true;
    else return false;
}

bool isOnRight(const int lsValue) {
    if(lsValue & 0b11100000)
        return true;
    else return false;
}

bool isOnCross(const int lsValue) {
    return false;
}

bool isOnTCross(const int lsValue) {
    return false;
}

bool isOnRCorner(const int lsValue) {
    return isOnLeft(lsValue);
}

bool isOnLCorner(const int lsValue) {
    return isOnRight(lsValue);
}
