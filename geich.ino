#include "bldc.h"

const float K = 0.5; // коэффициент усреднения замеров
const float currentK = 7.82; //коэффициент, на который нужно делить значение analogRead чтоб получить ток в амперах
const int timer2period = 50000;
const int timer3period = 40;
const int currentlimit = 80; // максимальный ток


volatile float current = 0;
volatile int cnt = 0;
volatile float valA = 0;
volatile float valB = 0;
volatile float valC = 0;
volatile int tmpval = 0;
volatile bool vala = false;
volatile bool valb = false;
volatile bool valc = false;


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
const int CurrentPin = PB0;
//------------------------------------------------------------------------------------------------
unsigned long lastMillis = 0;
const int driverResetTimeout = 1000;
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
    //Аппаратное прерывание на пинах датчиков Холла
   /* attachInterrupt (HallA, hallInterrupt, CHANGE);
    attachInterrupt (HallB, hallInterrupt, CHANGE);
    attachInterrupt (HallC, hallInterrupt, CHANGE);*/
    //bldc.setOffset(3); //реверс

    Timer2.pause(); // останавливаем таймер перед настройкой
    Timer2.setPeriod(timer2period); // время в микросекундах
    Timer2.attachInterrupt(TIMER_UPDATE_INTERRUPT, func_tim_2); // активируем прерывание
    Timer2.refresh(); // обнулить таймер 
    Timer2.resume(); // запускаем таймер

    Timer3.pause(); // останавливаем таймер перед настройкой
    Timer3.setPeriod(timer3period); // время в микросекундах
    Timer3.attachInterrupt(TIMER_UPDATE_INTERRUPT, func_tim_3); // активируем прерывание
    Timer3.refresh(); // обнулить таймер 
    Timer3.resume(); // запускаем таймер
}

void loop() {
  /*tmpval = analogRead(HallA);
  valA = valA*(1-K) + tmpval*K;
  tmpval = analogRead(HallB);
  valB = valB*(1-K) + tmpval*K;
  tmpval = analogRead(HallC);
  valC = valC*(1-K) + tmpval*K;*/
  
  //pwmproc = analogRead(PEDAL) * 0.87912; //скважность зависит от потенциометра
  //pwmproc = 3600.0; //100% скважность
  /*bldc.setPWM(round(pwmproc));
  int tmpval = millis() - lastMillis;
  if(tmpval > driverResetTimeout)
  {
    bldc.reset();
  }

  tmpval = millis() - dbgMillis;
  if(tmpval > debugTimeout)
  {
    char outstr[15];
    dtostrf(bldc.getRPM(),7, 2, outstr);
    Serial.print("speed: ");
    Serial.print(outstr);
    Serial.println("rpm");
    float voltage = analogRead(VoltagePin) / 69.264;
    dtostrf(voltage,7, 2, outstr);
    Serial.print("voltage: ");
    Serial.print(voltage);
    Serial.println("V");

    Serial.print(valA > 2000 ? "1" : "0");
    Serial.print(" ");
    Serial.print(valB > 2000 ? "1" : "0");
    Serial.print(" ");
    Serial.println(valC > 2000 ? "1" : "0");
    

    dbgMillis = millis();*/
  }
}
void func_tim_2() {
  current = current * (1-K) + analogRead(CurrentPin)/7.82 * K;
  if(cnt > 20)
  {
    char outstr[15];
    dtostrf(current,7, 2, outstr);
    Serial.println(outstr);
  }
  cnt++;
}

void func_tim_3() {
  pwmproc = analogRead(PEDAL) * 0.87912; //скважность зависит от потенциометра
  //pwmproc = 3600.0; //100% скважность
  if(current > currentlimit){
    pwmproc = 0;
  }
  bldc.setPWM(round(pwmproc));
  //lastMillis = millis();
  valA = valA*(1-K) + analogRead(HallA)*K;
  valB = valB*(1-K) + analogRead(HallB)*K;
  valC = valC*(1-K) + analogRead(HallC)*K;
  bool nvala = (valA > 3000) ? true : false;
  bool nvalb = (valB > 3000) ? true : false;
  bool nvalc = (valC > 3000) ? true : false;
  if(nvala != vala || nvalb != valb || nvalc != valc)
  {
    bldc.setVals(nvala, nvalb, nvalc);
    bldc.myinterrupt();
    //lastMillis = millis();
  }
  vala = nvala; valb = nvalb; valc = nvalc;
  //bldc.setVals(valA > 2000 ? true : false , valB > 2000 ? true : false , valC > 2000 ? true : false);
  //bldc.myinterrupt();
}
