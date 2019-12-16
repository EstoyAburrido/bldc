#include "bldc.h"


BLDC::BLDC(int TAH, int TAL, int TBH, int TBL, int TCH, int TCL, int HallA, int HallB, int HallC) :
  _TAH(TAH),  _TAL(TAL),  _TBH(TBH),  _TBL(TBL),  _TCH(TCH),  _TCL(TCL),  _HallA(HallA),  _HallB(HallB),  _HallC(HallC)
{
  pinMode(_HallA, INPUT_PULLUP); // pinmode depends of actual circuit
  pinMode(_HallB, INPUT_PULLUP); // other modes might required by different circuits
  pinMode(_HallC, INPUT_PULLUP); 
  pinMode(_TAH, OUTPUT);
  pinMode(_TAL, PWM); //only lower hald-bridge transistors are working in PWM mode
  pinMode(_TBH, OUTPUT);
  pinMode(_TBL, PWM);
  pinMode(_TCH, OUTPUT);
  pinMode(_TCL, PWM);
  _tmpmillis = millis();
  _laststate = 0;
}

void BLDC::setPWM(int pwm){
  _pwm = pwm;
  myinterrupt();
}

void BLDC::reset()
{
  switchPhase(0, 0, 0, 0, 0, 0);
  delay(_timeout);
  _rpm = 0.0;
  myinterrupt();
}

void BLDC::setOffset(int offset)
{
  _offset = offset;
  myinterrupt();
}

void BLDC::setVals(bool vala, bool valb, bool valc) //временный костыль,
{// будет убран когда в контроллере будет собран low pass фильтр и значения с датчиков холла можно будет считывать напрямую
	_vala = vala;
	_valb = valb;
	_valc = valc;	
}

void BLDC::myinterrupt(){
    //_vala = digitalRead(_HallA);
    //_valb = digitalRead(_HallB);  
    //_valc = digitalRead(_HallC); 
    if(_vala && !_valb && _valc) {
      setStep(1);
    } if(_vala && !_valb && !_valc) {
      setStep(2);
    } if(_vala && _valb && !_valc) {
      setStep(3);
    } if(!_vala && _valb && !_valc) {
      setStep(4);
    } if(!_vala && _valb && _valc) {
      setStep(5);
    } if(!_vala && !_valb && _valc) {
      setStep(6);
    }
    if(_vala && !_laststate && millis() - _tmpmillis > 4) { //когда состояние датчика A сменилось с 0 на 1
    	_rpm = millis() - _tmpmillis;
    	_rpm = (60000 / _rpm)/4; // the number 4 here because in the motor we've been using there are 4 poles.
	    			// if you have a different number of poles you should change this number, otherwise
	    			// you'd get wrong RPM value
    	_tmpmillis = millis();
    }
    _laststate = _vala;
}

float BLDC::getRPM(){
	return _rpm;
}

void BLDC::setStep(int step){

  _step = step + _offset;
  if (_step > 6) { _step = _step - 6; }
  if (_step < 1) { _step = _step + 6; }

  switch(_step)
  {
      case 1:
          switchPhase(1, 0, 0, 1, 0, 0);
		  break;
      case 2:
          switchPhase(1, 0, 0, 0, 0, 1);    
		  break;
      case 3:
          switchPhase(0, 0, 1, 0, 0, 1);   
		  break;
      case 4:
          switchPhase(0, 1, 1, 0, 0, 0);    
		  break;
      case 5:
          switchPhase(0, 1, 0, 0, 1, 0);   
		  break;
      case 6:
          switchPhase(0, 0, 0, 1, 1, 0);   
		  break;
      default:
  		  switchPhase(0, 0, 0, 0, 0, 0);    
		  break;
  }
}

void BLDC::switchPhase(bool TAH, bool TAL, bool TBH, bool TBL, bool TCH, bool TCL)
{
  digitalWrite(_TAH, TAH ? HIGH : LOW);
  pwmWrite(_TAL, TAL ? _pwm : 0);
  digitalWrite(_TBH, TBH ? HIGH : LOW);
  pwmWrite(_TBL,  TBL ? _pwm : 0);
  digitalWrite(_TCH, TCH ? HIGH : LOW);
  pwmWrite(_TCL,  TCL ? _pwm : 0); 
}
