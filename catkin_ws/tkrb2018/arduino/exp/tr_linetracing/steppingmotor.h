#ifndef STEPPINGMOTOR_H
#define STEPPINGMOTOR_H
struct steppingMotorPinAssign {
    int cw;
    int ccw;
};

class SteppingMotor {
    public:
        SteppingMotor(steppingMotorPinAssign pin, float stepAngle);

        void controlBySpeed(int speed);
        void controlByAngle(int step);
        int getStep();
        void resetStep();

    private:
        int step; //resetせずに250マス進むとオーバーフローするよ！
        float stepAngle;
        steppingMotorPinAssign pin;
};
#endif
