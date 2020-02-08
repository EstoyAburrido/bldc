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

void BLDC::setPWM(int pwm){ // Метод задаёт скважность
  _pwm = pwm; // Записываем переданную в метод скважность в переменную _pwm
  myinterrupt(); // запускаем метод myinterrupt
}

void BLDC::setDelay(int delay){ // Метод задаёт задержку между шагами
  _delay = delay;
}

void BLDC::reset() // метод для перезапуска для перезарядки конденсаторов драйверов
{
  switchPhase(0, 0, 0, 0, 0, 0); // выключаем все пины
  delay(_timeout); // ждём
  //_rpm = 0.0;
  myinterrupt(); // запускаем метод myinterrupt
}

void BLDC::setOffset(int offset) // задаём смещение
{
  _offset = offset; // сохраняем смещение в переменную
  myinterrupt();
}

void BLDC::myinterrupt(){ // основной метод, корторый запускается при смене настроек, при срабатывании прерывания и т.п.
    _vala = digitalRead(_HallA); // читаем датчики
    _valb = digitalRead(_HallB);  
    _valc = digitalRead(_HallC); 
    if(_vala && !_valb && _valc) { // в зависимости от положения датчиков запускаем метод setStep с разными параметрами
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
    	_rpm = 60000 / _rpm; 	// тут считаем скорость и сохраняем её в переменную _rpm
    	_rpm = _rpm / 4;	// the number 4 here because in the motor we've been using there are 4 poles.
	    			// if you have a different number of poles you should change this number, otherwise
	    			// you'd get wrong RPM value
    	_tmpmillis = millis();
    }
    _laststate = _vala;
}

float BLDC::getRPM(){ // метод просто возвращает переменную _rpm которая хранит в себе текущую скорость
	return _rpm;
}

void BLDC::setStep(int step){ // в зависимости от переданного параметра(шаг) переключаем пины

  _step = step + _offset;
  while (_step > 6) { _step = _step - 6; } // если коряво задано смещение - двигаем шаг в зону от 1 до 6
  while (_step < 1) { _step = _step + 6; }
  static bool olda, oldb, oldc;
  if(_delay > 0 && (olda != _vala || oldb != _valb || oldcc != _valc)){ // если задана задержка и сменилось состояние датчиков
      switchPhase(0, 0, 0, 0, 0, 0); // выключаем все пины
      delayMicroseconds(_delay);
      olda = _vala; oldb = _valb; oldc = _valc;
  }
  switch(_step) // в зависимости от текущего шага включаем соответствующие пины
  {
      case 1:
          switchPhase(1, 0, 0, 1, 0, 0); // переключаем пины через метод switchPhase
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
{// этот метод включает заданные пины
  digitalWrite(_TAH, TAH ? HIGH : LOW);
  pwmWrite(_TAL, TAL ? _pwm : 0);
  digitalWrite(_TBH, TBH ? HIGH : LOW);
  pwmWrite(_TBL,  TBL ? _pwm : 0);
  digitalWrite(_TCH, TCH ? HIGH : LOW);
  pwmWrite(_TCL,  TCL ? _pwm : 0); 
}
