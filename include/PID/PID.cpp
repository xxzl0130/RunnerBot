#include"PID.h"

PID_TYPE limit(PID_TYPE x,PID_TYPE Min,PID_TYPE Max)
{
    return x > Max ? Max : (x < Min ? Min : x);
}

PID_TYPE PID::update(PID_TYPE error, PID_TYPE position)
{
    PID_TYPE pTerm,dTerm,iTerm,output;
    // calculate the proportional term
    pTerm = pGain * error;
    // calculate the integral state with appropriate limiting
    iState += error;
#ifdef ENABLE_INT_LIMIT
    iState = limit(iState,iMax,iMin);
#endif // ENABLE_INT_LIMIT
    iTerm = iGain * iState; // calculate the integral term
    dTerm = dGain * (position - dState);
    dState = position;
    output = pTerm + iTerm - dTerm;
#ifdef ENABLE_OUT_LIMIT
    output = limit(output,oMax,oMin);
#endif // ENABLE_OUT_LIMIT
    return output;
}

void PID::resetIntState()
{
    iState = 0.0;
}

void PID::setKp(PID_TYPE Kp)
{
    pGain = Kp;
}

void PID::setKi(PID_TYPE Ki)
{
    iGain = Ki;
}

void PID::setKd(PID_TYPE Kd)
{
    dGain = Kd;
}

#ifdef ENABLE_INT_LIMIT
void PID::setIntLimit(PID_TYPE Min,PID_TYPE Max)
{
    iMin = Min;
    iMax = Max;
}
#endif // ENABLE_INT_LIMIT
#ifdef ENABLE_OUT_LIMIT
void PID::setOutLimit(PID_TYPE Min,PID_TYPE Max)
{
    oMin = Min;
    oMax = Max;
}
#endif
