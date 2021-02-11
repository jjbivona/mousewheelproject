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


#define CLK 2
#define DATA 3
static uint8_t prevNextCode = 0;
static uint16_t store = 0;
int c = 0;
int oldc = 0;
String dirEnc = "-";
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);


//setting up timer

long OnTime = 5000;           // milliseconds of on-time
long OffTime = 10000;  // milliseconds of off-time
long StopTime = 2000; //don't think I need this one
int lockedpos = 120;
int openpos = 30;
//setting up servo name
Servo myservo;



void setup()
{
  u8g2.begin();
  u8g2.setFlipMode(1);

  pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT_PULLUP);
  Serial.begin (9600);
  Serial.println("Start:");
  attachInterrupt(0, interruptA, CHANGE); //Old method
  attachInterrupt(1, interruptB, CHANGE);
    //servo setup and initial position
  myservo.attach(9);
 

}
void loop()
{ 
  unsigned long currentMillis = millis(); //this needed to be within the loop for some reason
  if ((currentMillis <= OnTime)) {

    myservo.write(lockedpos);
         
 } //locks wheel from spinning
 //wheel unlocks here
  else if ((currentMillis >= OnTime) && (currentMillis <= OffTime)) {

    myservo.write(openpos);
    }
     else if (currentMillis >= OffTime)  {

    myservo.write(lockedpos);
    }

  


  if (c != oldc)

    oldc = c;
      u8g2.firstPage();     //displays "off" to screen when wheel is locked
    do {
      u8g2.setFont(u8g2_font_artosserif8_8r);
      u8g2.setCursor(10, 10);
      u8g2.print("Status:");
       if (myservo.read() == lockedpos){u8g2.setCursor(50, 20);
      u8g2.print("locked");}
      else {u8g2.print("unlocked");}
      u8g2.setCursor(10, 30);
      u8g2.print("Distance (m):");
      u8g2.setCursor(50, 40);
      u8g2.print((double)c / 20 * 3.14 * .06);
      u8g2.setCursor(10, 50);
      u8g2.print("time (s):");
      u8g2.setCursor(50, 60);
      u8g2.print((double)millis()/1000);
    } while ( u8g2.nextPage() );
  {
    Serial.print(c); Serial.print(" "); Serial.println(dirEnc);
    
  }
}
void interruptA()
{
  cli(); //stop interrupts
  read_rotary();
  sei(); //restart interrupts
}
void interruptB()
{
  cli(); //stop interrupts
  read_rotary();
  sei(); //restart interrupts
}
void read_rotary() // A vald CW or CCW move returns 1, invalid returns 0.
{
  static int8_t rot_enc_table[] = {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0};
  prevNextCode <<= 2;
  if (digitalRead(CLK)) prevNextCode |= 0x02;
  if (digitalRead(DATA)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;
  if (rot_enc_table[prevNextCode] ) // If valid then store as 16 bit data.
  {
    store <<= 4; //shift bits in 'store' 4 left ie *16
    store |= prevNextCode; //bitwise or, x |= y; equiv to x = x | y;
    if ((store & 0xff) == 0x2b)
    {
      dirEnc = ("<");
      c++;
    }
    if ((store & 0xff) == 0x17)
    {
      dirEnc = (">");
      c++;
    }
  }
}
