#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Stepper.h>
#include <SPI.h>

#include <TimeLib.h>
#include <WidgetRTC.h>
#include <ArduinoOTA.h>


BlynkTimer timer;
WidgetRTC rtc;


uint8_t wire5 = D8;
uint8_t wire6 = D7;
uint8_t wire7 = D6;
uint8_t wire8 = D5;
uint8_t wire1 = D4;
uint8_t wire2 = D3;
uint8_t wire3 = D2;
uint8_t wire4 = D1;

char auth[] = "7da4008602f84a50a664687027d5b4a8";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "2.4G-Vectra-WiFi-97A353";
char pass[] = "pnl7okqdluz7cpiw";

int val;
int val1;
int absl;

const int MAX_SCHEDULE = 2;          //number of available schedules
const int START_TIMER_PIN = V10;      //START_TIMER_PIN is the virtual pin of  
unsigned long startseconds[MAX_SCHEDULE]; //the FIRST timer input pin. In case of  
unsigned long stopseconds[MAX_SCHEDULE];    //MAX_SCHEDULE = 3, the widget pins 
bool scheduleWeekDay[MAX_SCHEDULE][7];    //will be V10, V11 and V12

const uint16_t _delay = 1; /* delay in between two steps. minimum delay more the rotational speed */

void sequence(bool a, bool b, bool c, bool d){  /* four step sequence to stepper motor */
  digitalWrite(wire1, a);
  digitalWrite(wire2, b);
  digitalWrite(wire3, c);
  digitalWrite(wire4, d);
  delay(_delay);
}

void sequence1(bool a, bool b, bool c, bool d){  /* four step sequence to stepper motor */
  digitalWrite(wire5, a);
  digitalWrite(wire6, b);
  digitalWrite(wire7, c);
  digitalWrite(wire8, d);
  delay(_delay);
}

void setSchedule(const BlynkParam& param, int nSchedule) {         
  TimeInputParam t(param);
  startseconds[nSchedule] =  t.getStartHour()*3600 + t.getStartMinute()*60;
  stopseconds[nSchedule]  =  t.getStopHour()*3600  + t.getStopMinute()*60;
  for(int day = 1; day <=7; day++) { scheduleWeekDay[nSchedule][day%7] = t.isWeekdaySelected(day); } //Time library starts week on Sunday=1, Blynk on Monday=1, need to translate Blynk value to time value!! AND need to run from 0-6 instead of 1-7
}

void checkSchedule(){
  for (int nSchedule=0; nSchedule<MAX_SCHEDULE; nSchedule++){
    if( scheduleWeekDay[nSchedule][weekday()-1] ){    //Schedule is ACTIVE today
      unsigned long nowseconds =  hour()*3600 + minute()*60 + second();
  
      if(nowseconds >= startseconds[nSchedule] - 31 && nowseconds <= startseconds[nSchedule] + 31 ){  // 62s on 60s timer ensures 1 trigger command is sent
        //PUT YOUR START ACTIVITY CODE HERE!!
        Serial.println("Schedule started");
for(i=0; i<10000; i++)
{
    sequence(HIGH, LOW, LOW, LOW);
    sequence(HIGH, HIGH, LOW, LOW);
    sequence(LOW, HIGH, LOW, LOW);
    sequence(LOW, HIGH, HIGH, LOW);
    sequence(LOW, LOW, HIGH, LOW);
    sequence(LOW, LOW, HIGH, HIGH);
    sequence(LOW, LOW, LOW, HIGH);
    sequence(HIGH, LOW, LOW, HIGH);

    sequence1(HIGH, LOW, LOW, LOW);
    sequence1(HIGH, HIGH, LOW, LOW);
    sequence1(LOW, HIGH, LOW, LOW);
    sequence1(LOW, HIGH, HIGH, LOW);
    sequence1(LOW, LOW, HIGH, LOW);
    sequence1(LOW, LOW, HIGH, HIGH);
    sequence1(LOW, LOW, LOW, HIGH);
    sequence1(HIGH, LOW, LOW, HIGH);
}
      }
  
      if(nowseconds >= stopseconds[nSchedule] - 31 && nowseconds <= stopseconds[nSchedule] + 31 ){
        //PUT YOUR STOP ACTIVITY CODE HERE!!
        Serial.println("Schedule ended");
for(i=0; i<10000; i++)
{
    sequence1(LOW, LOW, LOW, HIGH);
    sequence1(LOW, LOW, HIGH, HIGH);
    sequence1(LOW, LOW, HIGH, LOW);
    sequence1(LOW, HIGH, HIGH, LOW);
    sequence1(LOW, HIGH, LOW, LOW);
    sequence1(HIGH, HIGH, LOW, LOW);
    sequence1(HIGH, LOW, LOW, LOW);
    sequence1(HIGH, LOW, LOW, HIGH);

    sequence(LOW, LOW, LOW, HIGH);
    sequence(LOW, LOW, HIGH, HIGH);
    sequence(LOW, LOW, HIGH, LOW);
    sequence(LOW, HIGH, HIGH, LOW);
    sequence(LOW, HIGH, LOW, LOW);
    sequence(HIGH, HIGH, LOW, LOW);
    sequence(HIGH, LOW, LOW, LOW);
    sequence(HIGH, LOW, LOW, HIGH);
}
      }//seconds
    }//day
  }//for
}//checkSchedule

BLYNK_CONNECTED() {
  rtc.begin();
}

BLYNK_WRITE_DEFAULT(){  //Read widget updates coming in from APP
  int pin = request.pin;
   if(pin >= START_TIMER_PIN && pin <= (START_TIMER_PIN + MAX_SCHEDULE) ){
      int nSchedule = pin - START_TIMER_PIN;
      Serial.println(String("Schedule ") + nSchedule + "update");
      setSchedule(param,nSchedule);
  }
  //you can add more vpin X checks in this route OR use BLYNK_WRITE(VX) as usual
}

void setup() {
  Serial.println("Booting");
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) {  }   // Wait until connected
  Serial.println("-------------");
  Serial.println(String("Blynk v") + BLYNK_VERSION + " started");
  Serial.println("-------------");
  pinMode(wire1, OUTPUT); /* set four wires as output */
  pinMode(wire2, OUTPUT);
  pinMode(wire3, OUTPUT);
  pinMode(wire4, OUTPUT);
  pinMode(wire5, OUTPUT); /* set four wires as output */
  pinMode(wire6, OUTPUT);
  pinMode(wire7, OUTPUT);
  pinMode(wire8, OUTPUT);
  //===OTA
  ArduinoOTA.setHostname("MultiScheduler");       
  ArduinoOTA.begin();    
  //===OTA
  
  //===TIMERS
  timer.setInterval(60*1000, checkSchedule);  // check schedule every 1 minute
  //===TIMERS
}

BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  val = pinValue;
  absl = abs(val);
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  val1 = pinValue;
  absl = abs(val1);
}

BLYNK_WRITE(V3) 
{
  if (param.asInt() == 1) {
    val = 1;
  }
  else {
    val = 0;
  }
}

void loop() {
  Blynk.run();
  timer.run(); 
  if(Blynk.connected()) { Blynk.run(); }
  ArduinoOTA.handle()
  /* Rotation in one direction */
  if(val>0)
  //for(int i = 0; i<12; i++)
  {
    sequence(HIGH, LOW, LOW, LOW);
    sequence(HIGH, HIGH, LOW, LOW);
    sequence(LOW, HIGH, LOW, LOW);
    sequence(LOW, HIGH, HIGH, LOW);
    sequence(LOW, LOW, HIGH, LOW);
    sequence(LOW, LOW, HIGH, HIGH);
    sequence(LOW, LOW, LOW, HIGH);
    sequence(HIGH, LOW, LOW, HIGH);
  }
    /* Rotation in one direction */
  if(val1>0)
  //for(int i = 0; i<12; i++)
  {
    sequence1(HIGH, LOW, LOW, LOW);
    sequence1(HIGH, HIGH, LOW, LOW);
    sequence1(LOW, HIGH, LOW, LOW);
    sequence1(LOW, HIGH, HIGH, LOW);
    sequence1(LOW, LOW, HIGH, LOW);
    sequence1(LOW, LOW, HIGH, HIGH);
    sequence1(LOW, LOW, LOW, HIGH);
    sequence1(HIGH, LOW, LOW, HIGH);
  }
    if(val1<0)
  {
    sequence1(LOW, LOW, LOW, HIGH);
    sequence1(LOW, LOW, HIGH, HIGH);
    sequence1(LOW, LOW, HIGH, LOW);
    sequence1(LOW, HIGH, HIGH, LOW);
    sequence1(LOW, HIGH, LOW, LOW);
    sequence1(HIGH, HIGH, LOW, LOW);
    sequence1(HIGH, LOW, LOW, LOW);
    sequence1(HIGH, LOW, LOW, HIGH);
  }

    if(val<0)
  {
    sequence(LOW, LOW, LOW, HIGH);
    sequence(LOW, LOW, HIGH, HIGH);
    sequence(LOW, LOW, HIGH, LOW);
    sequence(LOW, HIGH, HIGH, LOW);
    sequence(LOW, HIGH, LOW, LOW);
    sequence(HIGH, HIGH, LOW, LOW);
    sequence(HIGH, LOW, LOW, LOW);
    sequence(HIGH, LOW, LOW, HIGH);
  }

  if(val==0)
  sequence(LOW, LOW, LOW, LOW);
  if(val1==0)
  sequence1(LOW, LOW, LOW, LOW);
  /* Rotation in opposite direction
  

 */

}
