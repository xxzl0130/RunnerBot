#ifndef __DC_MOTOR_H__
#define __DC_MOTOR_H__

#include <Arduino.h>
#include <math.h>

#ifndef uchr
#define uchr unsigned char
#endif

#define SPEED_MAX 255
#define SPEED_MIN 0

// 角度制转弧度制
#define degree2radian(angle) ((angle) / 180.0 * M_PI)

enum Direction
{
    FORWORD = 0,BACKWORD
};

struct pairSpeed
{
    uchr xSpeed,ySpeed;
    Direction xDir,yDir;
};

class DC_Motor
{
protected:
    uchr Pin[2];
    uchr currentSpeed;
    Direction currentDir;
public:
    // 初始化参数为两个PWM控制引脚
    DC_Motor(uchr Pin0,uchr Pin1):currentSpeed(0),currentDir(FORWORD)
    {
        Pin[0] = Pin0;
        Pin[1] = Pin1;

        pinMode(Pin[0],OUTPUT);
        digitalWrite(Pin[0],LOW);

        pinMode(Pin[1],OUTPUT);
        digitalWrite(Pin[1],LOW);
    }

    // 运动
    void run(Direction Dir,uchr Speed)
    {
        currentDir = Dir;
        currentSpeed = Speed;
        //存储当前的速度与方向
        analogWrite(Pin[Dir],Speed);
        digitalWrite(Pin[Dir ^ 1],LOW);
    }

    // 停止
    void stop()
    {
        digitalWrite(Pin[0],HIGH);
        digitalWrite(Pin[1],HIGH);
        currentSpeed = 0;
    }

    // 加速
    void speedUp(int plusSpeed)
    {
        if((int)currentSpeed + plusSpeed > SPEED_MAX)
        {
            currentSpeed = SPEED_MAX;
        }
        else if(plusSpeed < 0 && -plusSpeed > currentSpeed)
        {
            currentSpeed = SPEED_MIN;
        }
        //保证加减后的速度还在0-255内

        run(currentDir,currentSpeed);
    }

    // 反转
    inline void reverse()
    {
        run(currentDir == FORWORD ? BACKWORD : FORWORD,currentSpeed);
    }

    // 获取当前速度
    inline uchr getCurrentSpeed(void)const
    {
        return currentSpeed;
    }

    // 获取当前方向
    inline Direction getCurrentDir(void)const
    {
        return currentDir;
    }
};

// 带有EN控制口的直流电机
class DC_Motor_EN:public DC_Motor
{
protected:
    uchr EN_Pin;
public:
    // 初始化参数为两个PWM控制引脚和EN使能引脚
    DC_Motor_EN(uchr Pin0,uchr Pin1,uchr enPin):DC_Motor(Pin0,Pin1),EN_Pin(enPin)
    {
        pinMode(EN_Pin,OUTPUT);
        digitalWrite(EN_Pin,HIGH);
    }

    // 启用电机
    inline void enable()
    {
        digitalWrite(EN_Pin,HIGH);
    }

    // 停用电机
    inline void disable()
    {
        digitalWrite(EN_Pin,LOW);
    }
};

class DC_Motor_EN_1:public DC_Motor_EN
{
public:
    DC_Motor_EN_1(uchr Pin0,uchr Pin1,uchr enPin):DC_Motor_EN(Pin0,Pin1,enPin)
    {
    }

    void run(Direction dir,uchr Speed)
    {
        digitalWrite(Pin[dir],HIGH);
        digitalWrite(Pin[dir ^ 1],LOW);
        analogWrite(DC_Motor_EN::EN_Pin,Speed);
        DC_Motor_EN::currentDir = dir;
        DC_Motor_EN::currentSpeed = Speed;
    }
};

// 左右一对电机
template <typename T>
class DC_MotorPair
{
protected:
    uchr currentSpeed;
    Direction currentDir;
    bool rotating;
public:
    T left,right;

    DC_MotorPair(T a,T b):currentSpeed(0),currentDir(FORWORD),rotating(false),left(a),right(b)
    {
        //使用传入的DC_Motor_EN对象（或其基类DC_Motor对象）初始化
    }

    // 直线运动
    inline void run(Direction Dir,uchr Speed)
    {
        left.run(Dir,Speed);
        right.run(Dir,Speed);
        currentSpeed = Speed;
        currentDir = Dir;
        rotating = false;
    }

    // 旋转
    inline void rotateRun(Direction Dir,uchr Speed)
    {
        left.run(Dir,Speed);
        right.run(Dir == FORWORD ? BACKWORD : FORWORD,Speed);
        rotating = true;
    }

    // 反向
    inline void reverse()
    {
        left.reverse();
        right.reverse();
        currentDir = currentDir == FORWORD ? BACKWORD : FORWORD;
    }

    // 加速
    inline void speedUp(int plusSpeed)
    {
        left.speedUp(plusSpeed);
        right.speedUp(plusSpeed);
    }

    // 加速旋转
    inline void rotateSpeedUp(int plusSpeed)
    {
        left.speedUp(plusSpeed);
        right.speedUp(-plusSpeed);
    }

    inline void stop()
    {
        left.stop();
        right.stop();
        currentSpeed = 0;
    }

    inline uchr getCurrentSpeed(void)const
    {
        return currentSpeed;
    }

    inline Direction getCurrentDir(void)const
    {
        return currentDir;
    }

    inline bool isRotating(void)const
    {
        return rotating;
    }
};

// 垂直的四个轮子
template <typename T>
class DC_MotorVerticalSquare
{
protected:
    uchr currentSpeed;
    float currentAngle;
    bool rotating;
public:
    DC_MotorPair<T> xAxis,yAxis;

    DC_MotorVerticalSquare(DC_MotorPair<T> x,DC_MotorPair<T> y):currentSpeed(0),currentAngle(0),rotating(false),xAxis(x),yAxis(y)
    {
    }

    inline pairSpeed angleRunSpeed(float &angle,uchr &speed)
    {
        float xAxisSpeed,yAxisSpeed;
        pairSpeed speedRet;

        angle = degree2radian(angle);
        xAxisSpeed = cos(angle);
        yAxisSpeed = sin(angle);

        speedRet.xDir = xAxisSpeed >= 0.0 ? FORWORD : BACKWORD;
        speedRet.yDir = yAxisSpeed >= 0.0 ? FORWORD : BACKWORD;

        xAxisSpeed = fabs(xAxisSpeed);
        yAxisSpeed = fabs(yAxisSpeed);

        if(xAxisSpeed >= yAxisSpeed)
        {
            yAxisSpeed /= xAxisSpeed;
            xAxisSpeed = 1.0;
        }
        else
        {
            xAxisSpeed /= yAxisSpeed;
            yAxisSpeed = 1.0;
        }

        speedRet.xSpeed = speed * xAxisSpeed;
        speedRet.ySpeed = speed * yAxisSpeed;

        return speedRet;
    }

    inline void angleRun(float angle,uchr speed)
    {
        pairSpeed spd;

        spd = angleRunSpeed(angle,speed);

        xAxis.run(spd.xDir,spd.xSpeed);
        yAxis.run(spd.yDir,spd.ySpeed);

        rotating = false;
        currentAngle = angle;
        currentSpeed = speed;
    }

    inline void speedUp(int xPlusSpeed,int yPlusSpeed)
    {
        xAxis.speedUp(xPlusSpeed);
        yAxis.speedUp(yPlusSpeed);
    }

    inline void rotateSpeedUp(int plusSpeed)
    {
        xAxis.rotateSpeedUp(plusSpeed);
        yAxis.rotateSpeedUp(plusSpeed);
    }

    inline void stop()
    {
        xAxis.stop();
        yAxis.stop();
    }

    inline void rotateRun(Direction dir,uchr speed)
    {
        xAxis.rotateRun(dir,speed);
        yAxis.rotateRun(dir,speed);
        currentSpeed = speed;
        rotating = true;
    }

    inline void reverse()
    {
        xAxis.reverse();
        yAxis.reverse();
    }

    inline void run(float angle,uchr speed,uchr omega,Direction dir)
    {
        angleRun(angle,speed);
        if(omega)
        {
            if(dir == FORWORD)
            {
                speedUp(omega,-omega);
            }
            else
            {
                speedUp(-omega,omega);
            }
        }
    }

    inline uchr getCurrentSpeed(void)const
    {
        return currentSpeed;
    }

    inline float getCurrentAngle(void)const
    {
        return currentAngle;
    }

    inline bool isRotating(void)const
    {
        return rotating;
    }
};

#endif //__DC_MOTOR_H__
