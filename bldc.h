#ifndef bldc_lib
#define bldc_lib
#include "Arduino.h"

class BLDC {
  public:
  	BLDC();
    BLDC(int TAH, int TAL, int TBH, int TBL, int TCH, int TCL, int HallA, int HallB, int HallC);
    void setOffset(int offset);
    void setPWM(int pwm);
    void reset();
    void myinterrupt();
    float getRPM();
 private:
    void switchPhase(bool TAH, bool TAL, bool TBH, bool TBL, bool TCH, bool TCL);   
    void setStep(int step);
    int _TAH, _TAL, _TBH, _TBL, _TCH, _TCL, _HallA, _HallB, _HallC, _step, _pwm;
    int _offset = 0;
    int _timeout = 5;
    int _delay = 0;
    volatile bool _vala;
    volatile bool _valb;
    volatile bool _valc;
    volatile bool _laststate;
    volatile unsigned long _tmpmillis = 0;
    volatile float _rpm = 0.0;
};

#endif
