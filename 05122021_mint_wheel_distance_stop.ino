#include <Servo.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
///////////////////////////////////////////////
//THIS IS ALL THAT NEEDS TO BE CHANGED/////////
///////////////////////////////////////////////

float threshold = 5; //distance in meters

//////////////////////////////////////////////
float distance;

int lockedpos = 120;  //servo position to lock
int openpos = 100; //servo position to unlock
Servo myservo; // servo
int hallsensor = 2; //hall sensor pin

volatile long count; //rotation count

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(A5, A4);

enum states
{
  lock,
  unlock,
  relocked
};

byte currentState;

unsigned long currentTime;
unsigned long printTime;

unsigned long relockedPeriod;



void setup()
{
  Serial.begin(115200);
  while (!Serial);
  currentState = lock;

  count = 0;

  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.setInverseFont(1);
  u8x8.setCursor(1, 0);
  u8x8.print("current status");

  u8x8.setCursor(1, 3);
  u8x8.print("distance (m)");
  u8x8.setCursor(1, 6);
  u8x8.print("time (min)");
  u8x8.setInverseFont(0);
  u8x8.setCursor(1, 1);
  u8x8.print("locked");

  attachInterrupt(digitalPinToInterrupt(hallsensor), sensor, FALLING); //attach interrupt
  pinMode(hallsensor, INPUT); //setup hall sensor
  myservo.attach(9); //setup pin for servo
  myservo.write(lockedpos); //default state is locked 
 
}

void loop()
{
  currentTime = millis();
  switch (currentState)
  {
    case lock:
      if (distance < threshold)
      {
        myservo.write(openpos); //open servo when condition reached
        
        
        currentState = unlock;
        u8x8.setCursor(1, 1);
        u8x8.print("unlocked"); //update screen

        break;
      }
    case unlock:
      if (distance > threshold)
      {
        myservo.write(lockedpos); //relocking the wheel
     
        
        currentState = relocked;
         u8x8.clearLine(1);
        u8x8.setCursor(1, 1);
        u8x8.print("locked"); //update screen
         u8x8.setCursor(10, 1);
        u8x8.print(printTime);
        break;
      }

  }
  //non blocking general code goes here
  screen();

}




void sensor () { //interrupt to measure rotations
  count++; //every time hall effect sensor goes off, add 1 to count
distance = (count * .054 * 2 * 3.14);
}

void screen () { //function to update screen
  Serial.println(count);


  u8x8.setCursor(1, 4);
  u8x8.print(count * .054 * 2 * 3.14); //distance in meters (2 * pi * r)


printTime = millis() / 1000 / 60 ; //time in minutes since power on 

  u8x8.setCursor(1, 7);
  u8x8.print(printTime); //printing time 
  
}
