#ifndef __PID_H__
#define __PID_H__

typedef double PID_TYPE;

#define ENABLE_INT_LIMIT 1
#define ENABLE_OUT_LIMIT 1

PID_TYPE limit(PID_TYPE x,PID_TYPE Min,PID_TYPE Max);

class PID
{
public:
    PID_TYPE dState; // Last position input
    PID_TYPE iState; // Integrator state
#ifdef ENABLE_INT_LIMIT
    PID_TYPE iMax, iMin; // Maximum and minimum allowable integrator state
#endif // ENABLE_INT_LIMIT
#ifdef ENABLE_OUT_LIMIT
    PID_TYPE oMax, oMin; // Maximum and minimum allowable output value
#endif // ENABLE_OUT_LIMIT
    PID_TYPE iGain, // integral gain
           pGain, // proportional gain
           dGain; // derivative gain
    PID() {}

    PID(PID_TYPE Kp = 0.0,PID_TYPE Ki = 0.0,PID_TYPE Kd = 0.0
#ifdef ENABLE_INT_LIMIT
        ,PID_TYPE _iMax = 1e7,PID_TYPE _iMin = -1e7
#endif // ENABLE_INT_LIMIT
#ifdef ENABLE_OUT_LIMIT
        ,PID_TYPE _oMax = 1e7,PID_TYPE _oMin = -1e7
#endif // ENABLE_OUT_LIMIT
        )
    {
        pGain = Kp;
        iGain = Ki;
        dGain = Kd;
#ifdef ENABLE_INT_LIMIT
        iMax = _iMax;
        iMin = _iMin;
#endif // ENABLE_INT_LIMIT
#ifdef ENABLE_OUT_LIMIT
        oMax = _oMax;
        oMin = _oMin;
#endif // ENABLE_OUT_LIMIT
        dState = 0.0;
		iState = 0.0;
    }

    PID_TYPE update(PID_TYPE error, PID_TYPE position);
    void resetIntState();
    void setKp(PID_TYPE Kp);
    void setKi(PID_TYPE Ki);
    void setKd(PID_TYPE Kd);
#ifdef ENABLE_INT_LIMIT
    void setIntLimit(PID_TYPE Min,PID_TYPE Max);
#endif // ENABLE_INT_LIMIT
#ifdef ENABLE_OUT_LIMIT
    void setOutLimit(PID_TYPE Min,PID_TYPE Max);
#endif
};

#endif // __PID_H__
