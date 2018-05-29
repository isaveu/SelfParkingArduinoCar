#include <hcsr04.h>
int encoder_pin = 2; // encoder pulse
unsigned long timeold;
int movementCount = 0;
int linearMotorA = A0;
int linearMotorB = A1;
int directionMotorA = A2;
int directionMotorB = A3;
HCSR04 ASensor; //Ön
HCSR04 BSensor; //Sağ orta
HCSR04 CSensor;//Arka Dik
HCSR04 DSensor;// Arka Sol
int A_TRIG=2;
int A_ECHO=3;
int B_TRIG=4;
int B_ECHO=5;
int C_TRIG=6;
int C_ECHO=7;
int D_TRIG=8;
int D_ECHO=9;
int IndicatorLed=13; // built-int
int Prosedur=0;
int HedefAralik=500;
enum MotorTip{
  Lineer,
  YonVerici
};
enum MesafeSensor{
  On,
  Sag,
  Arka,
  SolArka
};
void setup()
{
   Serial.begin(9600);
   pinMode(encoder_pin, INPUT);
   attachInterrupt(0, counter, FALLING);
   movementCount=0;
   timeold = 0;
   pinMode(directionMotorA,OUTPUT);
   pinMode(directionMotorB,OUTPUT);
   pinMode(linearMotorA,OUTPUT);
   pinMode(linearMotorB,OUTPUT);
   pinMode(IndicatorLed,OUTPUT);
   ASensor.init(A_TRIG, A_ECHO);
   BSensor.init(B_TRIG, B_ECHO);
   CSensor.init(C_TRIG,C_ECHO);
   DSensor.init(D_TRIG,D_ECHO);
}
void loop()
{
  ProsedurUygula(Prosedur);
}
void ProsedurUygula(int prosedur){
  switch(prosedur){
    case 0:
      if(MesafeOlc(Sag)<15){
        MotorHareket(Lineer,true);
      }else{
        Prosedur=1;
        movementCount = 0;
      }
    break;
    case 1:
      MotorHareket(Lineer,true);
      if(MesafeOlc(Sag)<15 && MesafeOlc(On)<2){
      Prosedur=7;
      }else{
        MesafeSay();
        if(movementCount>HedefAralik){
          Prosedur=2;
          movementCount=0;
        }
      }
    break;
    case 2:
      MotorHareket(Lineer,true);
      MotorHareket(YonVerici,false);
      MesafeSay();
      if(movementCount>300){
        Prosedur=3;
      }
    break;
    case 3:
      MotorHareket(Lineer,false);
      MotorHareket(YonVerici,true);
      if(MesafeOlc(SolArka)<25){
        Prosedur=4;
      }
    break;
    case 4:
      MotorHareket(Lineer,false);
      MotorHareket(YonVerici,false);
      if(MesafeOlc(SolArka)<=7){
        Prosedur=5;
      }
    break;
    case 5:
      if(MesafeOlc(On)>6){
        MotorHareket(Lineer,true);
        MotorHareket(YonVerici,true);
      }else{
        Prosedur=6;
      }
    break;
    case 6:
      if(abs(MesafeOlc(On)-MesafeOlc(Arka))<2){
        MotorHareket(Lineer,false);
        MotorHareket(YonVerici,false);
      }else{
        Prosedur=7;
        Serial.println("PARK COMPLETE");
      }
    break;
    case 7:
      digitalWrite(IndicatorLed,HIGH);
      delay(500);
      digitalWrite(IndicatorLed,LOW);
      delay(500);
    break;
  }
}
int MesafeOlc(MesafeSensor sensor){
  if(sensor==On){
    return ASensor.readDisctanceInCm();
  }else if(sensor==Sag){
    return BSensor.readDisctanceInCm();
  }else if(sensor==Arka){
    return CSensor.readDisctanceInCm();
  }else if(sensor==SolArka){
    return DSensor.readDisctanceInCm();
  }
  return 500; //safe point
}
void MotorHareket(MotorTip motorType,bool motorDir){
    if(motorDir){ //TRUE=>fwd || left
      digitalWrite(motorType==Lineer ? linearMotorA : directionMotorA,HIGH);
      digitalWrite(motorType==Lineer ? linearMotorB : directionMotorB,LOW);
    }else{
      digitalWrite(motorType==Lineer ? linearMotorA : directionMotorA,LOW);
      digitalWrite(motorType==Lineer ? linearMotorB : directionMotorB,HIGH);
    }
}
void MesafeSay(){
     if (millis() - timeold >= 1000) {
      //Don't process interrupts during calculations
      detachInterrupt(0);
      timeold = millis();
      //Serial.print("RPM = ");
      //Serial.println(rpm,DEC);
      //Restart the interrupt processing
      attachInterrupt(0, counter, FALLING);
   }
}
void counter()
{
   movementCount++;
   Serial.println(movementCount);
}
