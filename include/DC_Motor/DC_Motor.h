#ifndef __DC_MOTOR_H__
#define __DC_MOTOR_H__

#include <Arduino.h>
#include <math.h>

#ifndef uchr
#define uchr unsigned char
#endif

#define SPEED_MAX 255
#define SPEED_MIN 0

// �Ƕ���ת������
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
    // ��ʼ������Ϊ����PWM��������
    DC_Motor(uchr Pin0,uchr Pin1):currentSpeed(0),currentDir(FORWORD)
    {
        Pin[0] = Pin0;
        Pin[1] = Pin1;

        pinMode(Pin[0],OUTPUT);
        digitalWrite(Pin[0],LOW);

        pinMode(Pin[1],OUTPUT);
        digitalWrite(Pin[1],LOW);
    }

    // �˶�
    void run(Direction Dir,uchr Speed)
    {
        currentDir = Dir;
        currentSpeed = Speed;
        //�洢��ǰ���ٶ��뷽��
        analogWrite(Pin[Dir],Speed);
        digitalWrite(Pin[Dir ^ 1],LOW);
    }

    // ֹͣ
    void stop()
    {
        digitalWrite(Pin[0],HIGH);
        digitalWrite(Pin[1],HIGH);
        currentSpeed = 0;
    }

    // ����
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
        //��֤�Ӽ�����ٶȻ���0-255��

        run(currentDir,currentSpeed);
    }

    // ��ת
    void reverse()
    {
        run(currentDir == FORWORD ? BACKWORD : FORWORD,currentSpeed);
    }

    // ��ȡ��ǰ�ٶ�
    uchr getCurrentSpeed(void)const
    {
        return currentSpeed;
    }

    // ��ȡ��ǰ����
    Direction getCurrentDir(void)const
    {
        return currentDir;
    }
};

// ����EN���ƿڵ�ֱ�����
class DC_Motor_EN
{
protected:
	uchr Pin[2], EN_Pin;
	uchr currentSpeed;
	Direction currentDir;
public:
	// ��ʼ������Ϊ����PWM��������
	DC_Motor_EN(uchr Pin0, uchr Pin1, uchr enPin) :currentSpeed(0), currentDir(FORWORD)
	{
		Pin[0] = Pin0;
		Pin[1] = Pin1;
		EN_Pin = enPin;

		pinMode(Pin[0], OUTPUT);
		digitalWrite(Pin[0], LOW);

		pinMode(Pin[1], OUTPUT);
		digitalWrite(Pin[1], LOW);

		pinMode(EN_Pin, OUTPUT);
		digitalWrite(EN_Pin, LOW);
	}

	// �˶�
	void run(Direction Dir, uchr Speed)
	{
		currentDir = Dir;
		currentSpeed = Speed;
		//�洢��ǰ���ٶ��뷽��
		digitalWrite(Pin[Dir], HIGH);
		digitalWrite(Pin[Dir ^ 1], LOW);
		analogWrite(EN_Pin, Speed);
	}

	// ֹͣ
	void stop()
	{
		digitalWrite(Pin[0], HIGH);
		digitalWrite(Pin[1], HIGH);
		digitalWrite(EN_Pin, LOW);
		currentSpeed = 0;
	}

	// ����
	void speedUp(int plusSpeed)
	{
		if ((int)currentSpeed + plusSpeed > SPEED_MAX)
		{
			currentSpeed = SPEED_MAX;
		}
		else if (plusSpeed < 0 && -plusSpeed > currentSpeed)
		{
			currentSpeed = SPEED_MIN;
		}
		//��֤�Ӽ�����ٶȻ���0-255��

		run(currentDir, currentSpeed);
	}

	// ��ת
	void reverse()
	{
		run(currentDir == FORWORD ? BACKWORD : FORWORD, currentSpeed);
	}

	// ��ȡ��ǰ�ٶ�
	uchr getCurrentSpeed(void)const
	{
		return currentSpeed;
	}

	// ��ȡ��ǰ����
	Direction getCurrentDir(void)const
	{
		return currentDir;
	}
    // ���õ��
    void enable()
    {
        digitalWrite(EN_Pin,HIGH);
    }

    // ͣ�õ��
    void disable()
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

// ����һ�Ե��
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
        //ʹ�ô����DC_Motor_EN���󣨻������DC_Motor���󣩳�ʼ��
    }

    // ֱ���˶�
    void run(Direction Dir,uchr Speed)
    {
        left.run(Dir,Speed);
        right.run(Dir,Speed);
        currentSpeed = Speed;
        currentDir = Dir;
        rotating = false;
    }

    // ��ת
    void rotateRun(Direction Dir,uchr Speed)
    {
        left.run(Dir,Speed);
        right.run(Dir == FORWORD ? BACKWORD : FORWORD,Speed);
        rotating = true;
    }

    // ����
    void reverse()
    {
        left.reverse();
        right.reverse();
        currentDir = currentDir == FORWORD ? BACKWORD : FORWORD;
    }

    // ����
    void speedUp(int plusSpeed)
    {
        left.speedUp(plusSpeed);
        right.speedUp(plusSpeed);
    }

    // ������ת
    void rotateSpeedUp(int plusSpeed)
    {
        left.speedUp(plusSpeed);
        right.speedUp(-plusSpeed);
    }

    void stop()
    {
        left.stop();
        right.stop();
        currentSpeed = 0;
    }

    uchr getCurrentSpeed(void)const
    {
        return currentSpeed;
    }

    Direction getCurrentDir(void)const
    {
        return currentDir;
    }

    bool isRotating(void)const
    {
        return rotating;
    }
};

// ��ֱ���ĸ�����
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

    pairSpeed angleRunSpeed(float &angle,uchr &speed)
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

    void angleRun(float angle,uchr speed)
    {
        pairSpeed spd;

        spd = angleRunSpeed(angle,speed);

        xAxis.run(spd.xDir,spd.xSpeed);
        yAxis.run(spd.yDir,spd.ySpeed);

        rotating = false;
        currentAngle = angle;
        currentSpeed = speed;
    }

    void speedUp(int xPlusSpeed,int yPlusSpeed)
    {
        xAxis.speedUp(xPlusSpeed);
        yAxis.speedUp(yPlusSpeed);
    }

    void rotateSpeedUp(int plusSpeed)
    {
        xAxis.rotateSpeedUp(plusSpeed);
        yAxis.rotateSpeedUp(plusSpeed);
    }

    void stop()
    {
        xAxis.stop();
        yAxis.stop();
    }

    void rotateRun(Direction dir,uchr speed)
    {
        xAxis.rotateRun(dir,speed);
        yAxis.rotateRun(dir,speed);
        currentSpeed = speed;
        rotating = true;
    }

    void reverse()
    {
        xAxis.reverse();
        yAxis.reverse();
    }

    void run(float angle,uchr speed,uchr omega,Direction dir)
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

    uchr getCurrentSpeed(void)const
    {
        return currentSpeed;
    }

    float getCurrentAngle(void)const
    {
        return currentAngle;
    }

    bool isRotating(void)const
    {
        return rotating;
    }
};

#endif //__DC_MOTOR_H__
