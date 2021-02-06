//locking mouse wheel for exercise immunology research
//jj bivona
//march 2021


/* 
As the title suggests this is a mouse wheel for research purposes
    -mice will run 20km and for 7 hours if you let them 
    -this is not realistic for most humans
    -the wheel is designed to open for a time period (yet to be determined) when the mice are most active
    - it will stay open for x amount of time and the lock again to prevent additional exercise
    - the timer will be based on when you plug the power in. for example 4pm turn on for a 3am "exercise" 
I am having trouble getting the screen to update in real time
    - if I place the "do" loop (line 137) within the wheel unlocked portion of code (line 98) the data collection lags badly
    - for example, if you spin the rotary encoder fast enough, both the serial port and screen only record 1/2 to 1/3 of the clicks
    - because of this I have to put the data display after the wheel locks and is no longer moving
    - it would be nice to have real time updates instead of one at the end
    - it would also be nice to have a timer going until unlock, then how long the wheel has been unlocked, then how long it has been relocked.
*/


// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4

//other garbage
#include <Servo.h>
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//display setup
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);


//setting up variables for rotary encoder
float displaydistance = 0; //output float to the oled display
int counter = 0; //starting it at 0
int currentStateCLK;
int lastStateCLK;


//setting up timer

long OnTime = 5000;           // milliseconds of on-time
long OffTime = 20000;  // milliseconds of off-time
long StopTime = 2000; //don't think I need this one

//setting up servo name
Servo myservo;



void setup() {

  //start screen
  u8g2.begin();
  u8g2.setCursor(20, 20);

  //start serial monitor
  Serial.begin(9600);

  // Set encoder pins as inputs
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  //servo setup and initial position
  myservo.attach(9);
  myservo.write(120);

  //initialize rotary encoder
  lastStateCLK = digitalRead(CLK);
}

void loop() {

  unsigned long currentMillis = millis(); //this needed to be within the loop for some reason


  //wheel begins with it locked
  if ((currentMillis <= OnTime)) {

    myservo.write(120); //locks wheel from spinning


    u8g2.firstPage();     //displays "off" to screen when wheel is locked
    do {
      u8g2.setFont(u8g2_font_artosserif8_8r);
      u8g2.setCursor(40, 10);
      u8g2.print("off");
    } while ( u8g2.nextPage() );
  }

  //wheel unlocks here
  else if ((currentMillis >= OnTime) && (currentMillis <= OffTime)) {
    myservo.write(30);


    currentStateCLK = digitalRead(CLK); // Read the current state of CLK


    if (currentStateCLK != lastStateCLK  && currentStateCLK == 1) {  // If last and current state of CLK are different, then pulse occurred React to only 1 state change to avoid double count



      if (digitalRead(DT) != currentStateCLK) {  // If the DT state is different than the CLK state then the encoder is rotating and we need to add to the distance
        counter ++;
      } else {
        counter ++;
      }

      // printing to serial monitor - this doesn't affect readings
      Serial.print("revolutions ");
      Serial.println((double)counter / 20);
      Serial.println((double)counter / 20 * 3.14 * .06);
      Serial.println("meters ran");
      displaydistance = (double)counter / 20 * 3.14 * .06;




    }

    
    lastStateCLK = currentStateCLK; // Remember last CLK state

//timer is off, the wheel relocks to prevent movement 
  }
  else if (currentMillis >= OffTime)  {

    myservo.write(120); //servo locks the wheel

    //here we display final distance...I want to do this in realtime as it is spinning, but it chews up the ram 
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_artosserif8_8r);
      u8g2.setCursor(20, 10);
      u8g2.print("total distance(m)");
      u8g2.setCursor(40, 20);
      u8g2.print(displaydistance);
    } while ( u8g2.nextPage() );


    ;


  }

}
