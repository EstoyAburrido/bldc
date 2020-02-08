#include "bldc.h"

/*volatile bool vala = false; // это нужно если хочешь в сериал выводить текущее состояние датчиков
volatile bool valb = false;
volatile bool valc = false;*/

const int TAH = PB9; //T — транзистор, А — фаза (синяя), Н — верхний ключ полумоста
const int TAL = PA10; //T — транзистор, А — фаза (синяя), L — нижний ключ полумоста
const int TBH = PB8; //T — транзистор, B — фаза (зелёная), H — верхний ключ полумоста
const int TBL = PA9; //T — транзистор, B — фаза (зелёная), L — нижний ключ полумоста
const int TCH = PB7; //T — транзистор, C — фаза (жёлтая), H — верхний ключ полумоста
const int TCL = PA8; //T — транзистор, C — фаза (жёлтая), L — нижний ключ полумоста
const int HallA = PA2;
const int HallB = PA4;
const int HallC = PA6;
//------------------------------------------------------------------------------------------------
const int PEDAL = PB1;
//------------------------------------------------------------------------------------------------
unsigned long lastMillis = 0;
const int driverResetTimeout = 1000; // частота перезапуска (для перезарядки конденсаторов напряжения смещения драйвера)
//------------------------------------------------------------------------------------------------
unsigned long dbgMillis = 0;
const int debugTimeout = 500; // частота срабатывания принтов в loop()
//------------------------------------------------------------------------------------------------
float pwmproc;
HardwareTimer pwmtimer(1);
BLDC bldc(TAH, TAL, TBH, TBL, TCH, TCL, HallA, HallB, HallC); //создаём объект класса BLDC из либы bldc

void setup() {  
    Serial.begin(9600);      
    pwmtimer.setPeriod(50); //задаём частоту шим 20кгц, разрешение pwmWrite - 3600
    attachInterrupt (HallA, hallInterrupt, CHANGE);//Аппаратное прерывание на пинах датчиков Холла
    attachInterrupt (HallB, hallInterrupt, CHANGE);
    attachInterrupt (HallC, hallInterrupt, CHANGE);
    //bldc.setOffset(3); // Раскомментируй чтоб запустить двигатель в обратную сторону
}

void loop() {  
  //pwmproc = analogRead(PEDAL) * 0.87912; //скважность зависит от потенциометра
  pwmproc = 3600.0; //100% скважность
  bldc.setPWM(round(pwmproc)); // задаём эту скважность мотору
  int tmpval = millis() - lastMillis;
  if(tmpval > driverResetTimeout) // если прошло driverResetTimeout мсек - рестартим, чтоб перезарядились конденсаторы смещения напряжения
  {
    bldc.reset();
    lastMillis = millis();
  }
  
  /*tmpval = millis() - dbgMillis; //можно раскомментить если хочешь чтоб выводило текущую скорость в сериал
  if(tmpval > debugTimeout) 
  {
    char outstr[15];
    dtostrf(bldc.getRPM(),7, 2, outstr);
    Serial.print("speed: ");
    Serial.print(outstr);
    Serial.println("rpm");
    dbgMillis = millis();
  }*/
}

void hallInterrupt() {
  /*vala = digitalRead(HallA); // можно раскомментить если хочешь чтоб выводило текущее состояние датчиков в сериал
  Serial.print(vala);
  valb = digitalRead(HallB);
  Serial.print(valb);
  valc = digitalRead(HallC);
  Serial.println(valc);*/ 
  bldc.myinterrupt();
}

