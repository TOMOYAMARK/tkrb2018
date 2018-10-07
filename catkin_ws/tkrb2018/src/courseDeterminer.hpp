#pragma once

enum RoboCourse : char;
extern char GetCourse(const int lsValue);
//ラインセンサの生の値の配列を引数にとる。マジで許して。
//LineSensorクラスを用意して勝手に判断してくれるようにしたい

bool isOnCourse(const int lsValue);
bool isOnRight(const int lsValue);
bool isOnLeft(const int lsValue);
bool isOnRCorner(const int lsValue);
bool isOnLCorner(const int lsValue);
bool isOnCross(const int lsValue);
bool isOnTCross(const int lsValue);
