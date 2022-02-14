/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define relayPin 15
#define ledPin 13

  double batteryCutoffVoltage =  3.2;



// (C)
byte newCharCopyright[8] = {
  B11111,
  B10001,
  B10101,
  B10111,
  B10101,
  B10001,
  B11111,
  B00000
};

// (battery full)
byte batteryFull[8] = {
  B01110,
  B11111,
  B10001,
  B10101,
  B10101,
  B10101,
  B10001,
  B11111,
};

// (battery discharged)
byte batteryDischarged[8] = {
  B01110,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
};

enum TesterState {
  READY,
  TESTING,
  COMPLETE,
  ERROR,
};

  TesterState currentState = READY;
  TesterState desiredState = READY;


  long startTime = 0;
  long elapsedTime = 0;
  long cycleTime = 0;
  long cycleTimeOffset = 0;
  long finishTime = 0;

void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  lcd.createChar(1, newCharCopyright);
  lcd.createChar(2, batteryFull);
  lcd.createChar(3, batteryDischarged);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Battery tester");
  lcd.setCursor(0, 2);
  lcd.write(1);
  lcd.setCursor(2, 2);
  lcd.print("Joshua Moran");
  delay(2000);
  lcd.clear();
  digitalWrite(relayPin, LOW);


}

void loop() {
  cycleTime = millis()/1000 - startTime;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // print the number of seconds since reset:

  switch(currentState) {
    default: //catches 'READY'
      lcd.setCursor(0, 0);
      lcd.print("Tester Ready");
      lcd.setCursor(0, 2);
      lcd.print("[#] START");
      if(analogRead(A0) <= 700 && analogRead(A0) >= 600) {
        if(analogRead(A7)*(5.0/1023.0) >= batteryCutoffVoltage) {
          startTime = millis()/1000;
          lcd.clear();
          desiredState = TESTING;
        } else {
          desiredState = ERROR;
          currentState = desiredState; 
        }
      currentState = desiredState;
    break;
    case TESTING:
      
      digitalWrite(relayPin, HIGH);
      digitalWrite(ledPin, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("Discharging...");
      lcd.setCursor(15, 0);
      lcd.write(2);
      lcd.setCursor(0, 1);
      lcd.print((millis()/1000)-startTime);
      if(cycleTime - cycleTimeOffset >= 29) {
        Serial.println(analogRead(A7)*(5.0/1023.0));
        cycleTimeOffset = cycleTimeOffset + 30;
        lcd.clear();
        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Testing...");
        lcd.setCursor(15, 0);
        lcd.write(2);
        lcd.setCursor(0, 1);
        lcd.print((millis()/1000)-startTime);
        delay(1000);
        if(analogRead(A7)*(5.0/1023.0) <= batteryCutoffVoltage) {
          lcd.clear();
          finishTime = millis()/1000 - startTime;
          desiredState = COMPLETE;
          currentState = desiredState;
        }
        lcd.clear();
      }
    break;
    case COMPLETE:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Test complete!");
      lcd.setCursor(15, 0);
      lcd.write(3);
      lcd.setCursor(0, 1);
      lcd.print("Time ");
      lcd.setCursor(6, 1);
      lcd.print(finishTime);
      delay(2000);
    break;
  }
}
}

