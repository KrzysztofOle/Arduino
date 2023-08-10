
/*************************************************************************************

  Mark Bramwell, July 2010

  This program will test the LCD panel and the buttons.When you push the button on the shield，
  the screen will show the corresponding one.
 
  Connection: Plug the LCD Keypad to the UNO(or other controllers)

**************************************************************************************/
#include <AccelStepper.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  // select the pins used on the LCD panel



// define some values used by the panel and buttons
int lcd_key = 0;
int adc_key_in = 0;

// przyciski podłączone są do pojedynczego wejścia analogowego A0
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

bool lastUp = true;

// piny wykozystywane do sterowania LCD
// D4~D7,      D4~D7,     (4)
// RS.  ,      D8,
// Enable,     D9,
// Light,      D10,
//

#define pinSTP 19
#define pinDIR 18
#define pinENA 17

int moveStep = 16000;
int moveMax = 400;
int moveSpeed = 50;
int moveAcc = 50000;
int menu = 0;

#define menuRun 0
#define menuStep 1
#define menuMax 2
#define menuAcc 3
#define menuSave 4
#define menuJog 5
#define menuEnd 5

#define UP 1
#define DOWN -1

#define menuValueCol 11

float skokSroby = 12;     //mm
int stepOnRev = 200;      // ilośc koroków silnika na obrut
int microStep = 8;        // podniał na mikrokoroki w sterowniku
float stepDistance = 0;   //mm
float moveStepReal = 0;   // ruch w mm
int reaPosition = 0;
float moveJogReal = 10;     //mm
int moveJogSteps = 0;

void writeSet() {
  int eeAddress = 0;
  EEPROM.put(eeAddress, moveStep);
  eeAddress += sizeof(int);  //Move address to the next byte after
  EEPROM.put(eeAddress, moveMax);
  eeAddress += sizeof(int);  //Move address to the next byte after
  EEPROM.put(eeAddress, moveAcc);
  Serial.println("writeSet: Parametry zapisane w pamięci EEPROM");
}

void readSet() {
  int eeAddress = 0;
  EEPROM.get(eeAddress, moveStep);
  eeAddress += sizeof(int);  //Move address to the next byte after
  EEPROM.get(eeAddress, moveMax);
  eeAddress += sizeof(int);  //Move address to the next byte after
  EEPROM.get(eeAddress, moveAcc);
  Serial.println("readSet: Parametry odczytane z pamięci EEPROM");
}



// sterowanie silnikoem krokowym
AccelStepper stepper(AccelStepper::DRIVER,
                     pinSTP,
                     pinDIR,
                     true);

////stepper.

AccelStepper setEnablePin(pinENA);

int read_LCD_buttons() {       // read the buttons
  adc_key_in = analogRead(0);  // read the value from the sensor
  //Serial.println("adc_key_in: ");
  //Serial.println(adc_key_in);
  //delay(200);
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  // We make this the 1st option for speed reasons since it will be the most likely result

  if (adc_key_in > 1000) return btnNONE;

  // kalibracja 0, 100, 255, 408, 640

  // For V1.1 us this threshold
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in < 150) return btnUP;
  if (adc_key_in < 320) return btnDOWN;
  if (adc_key_in < 500) return btnLEFT;
  if (adc_key_in < 850) return btnSELECT;

  // For V1.0 comment the other threshold and use the one below:
  /*
     if (adc_key_in < 50)   return btnRIGHT;  
     if (adc_key_in < 195)  return btnUP; 
     if (adc_key_in < 380)  return btnDOWN; 
     if (adc_key_in < 555)  return btnLEFT; 
     if (adc_key_in < 790)  return btnSELECT;   
   */

  return btnNONE;  // when all others fail, return this.
}


int realToStep(float real) {
  int result = 0;
  result = real/stepDistance;
  return result;
}


int stepToReal(int step) {
  int result = 0;
  result = step*stepDistance;
  return result;
}


void setup() {
  pinMode(pinENA, OUTPUT);
  pinMode(pinDIR, OUTPUT);
  pinMode(pinSTP, OUTPUT);

  Serial.begin(9600);  // set up Serial library at 9600 bps
  Serial.println("");
  Serial.println("Stepper controler: ...");

  lcd.begin(16, 2);     // start the library
  lcd.setCursor(0, 0);  // set the LCD cursor   position
  //lcd.print("................");  // print a simple message on the LCD

  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(moveMax);
  stepper.setSpeed(moveSpeed);
  stepper.setAcceleration(moveAcc);
  //stepper.moveTo(500);
  // odczytujemy zapisane w EEPROM parametry
  //readSet();

  stepDistance = skokSroby / (stepOnRev * microStep);
  Serial.print("stepDistance: ");
  Serial.print(stepDistance*1000);
  Serial.println(" um");

  moveJogSteps = realToStep(moveJogReal);
  Serial.print("moveJogSteps: ");
  Serial.println(moveJogSteps);

}

void lcd_menu() {
  lcd.setCursor(0, 0);  // set the LCD cursor   position
  switch (menu) {
    case menuRun:
      {
        //lcd.print("menuRun         ");
        lcd.print(" SEL <--  --> RST");
        break;
      }
    case menuStep:
      {
        lcd.print("     step: ");
        lcd.setCursor(menuValueCol, 0);
        lcd.print(moveStep);
        lcd.print("   ");
        break;
      }
    case menuMax:
      {
        lcd.print("max speed: ");
        lcd.setCursor(menuValueCol, 0);
        lcd.print(moveMax);
        lcd.print("   ");
        break;
      }
    case menuAcc:
      {
        lcd.print("      acc: ");
        lcd.setCursor(menuValueCol, 0);
        lcd.print(moveAcc);
        lcd.print("   ");
        break;
      }
    case menuSave:
      {
        lcd.print("save param: DOWN");
      }
    case menuJog:
      {
        lcd.print("      jog: ");
        lcd.setCursor(menuValueCol, 0);
        lcd.print(moveAcc);
        lcd.print("   ");
      }
    default:
      {
        lcd.print("                ");
      }
  }
}



void buttonUpDown(int wsp) {

  switch (menu) {
    case menuRun:
      {
        break;
      }
    case menuStep:
      {
        if (lastUp) {
          lastUp = false;
          moveStep = moveStep + (wsp * 10);
        }
        break;
      }
    case menuMax:
      {
        if (lastUp) {
          lastUp = false;
          moveMax = moveMax + (wsp * 10);
          stepper.setMaxSpeed(moveMax);
        }
        break;
      }
    case menuAcc:
      {
        if (lastUp) {
          lastUp = false;
          moveAcc = moveAcc + (wsp * 1);
          stepper.setAcceleration(moveAcc);
        }
        break;
      }
    case menuSave:
      {
        if (lastUp) {
          lastUp = false;
          writeSet();
          lcd.setCursor(0, 1);
          lcd.print("parameters saved");
          delay(1000);
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }
        break;
      }
    case menuJog:
    {

    }
  }
}


void loop() {
  lcd_menu();
  lcd.setCursor(12, 1);                  // move cursor to second line "1" and 9 spaces over
  reaPosition = stepToReal(stepper.currentPosition());
  lcd.print(reaPosition);  // display seconds elapsed since power-up

  lcd.setCursor(0, 1);           // move to the begining of the second line
  lcd_key = read_LCD_buttons();  // read the buttons
  int move = 0;
  switch (lcd_key) {  // depending on which button was pushed, we perform an action

    case btnRIGHT:
      {  //  push button "RIGHT" and show the word on the screen
        if(menu != menuJog) {
          move = -moveStep;
        } else {
          move = -moveJogSteps;
        }
        lcd.print("move: ");
        lcd.print(move);
        stepper.move(move);
        delay(5);
        while (stepper.distanceToGo() != 0) {
          stepper.run();
          //lcd.setCursor(12, 1);
          //lcd.print(stepper.currentPosition());
          //  Serial.println(stepper.distanceToGo());
        }
        reaPosition = stepToReal(stepper.currentPosition());
        Serial.print("reaPosition: ");
        Serial.println(reaPosition);
        break;
      }
    case btnLEFT:
      {
        if(menu != menuJog) {
          move = moveStep;
        } else {
          move = moveJogSteps;
        }
        lcd.print("move: -");
        lcd.print(move);
        stepper.move(move);
        delay(5);
        while (stepper.distanceToGo() != 0) {
          stepper.run();
          //lcd.setCursor(12, 1);
          //lcd.print(stepper.currentPosition());
        }
        reaPosition = stepToReal(stepper.currentPosition());
        Serial.print("reaPosition: ");
        Serial.println(reaPosition);
        break;
      }
    case btnUP:
      {
        buttonUpDown(UP);
        break;
      }
    case btnDOWN:
      {
        buttonUpDown(DOWN);
        break;
      }
    case btnSELECT:
      {
        if (lastUp) {
          menu = menu + 1;
          if (menu > menuEnd) {
            menu = 0;
          }
          lastUp = false;
          //delay(10)
          break;
        }
      }
    case btnNONE:
      {
        lcd.print("NONE       ");  //  No action  will show "None" on the screen
        lastUp = true;
        break;
      }
  }
  stepper.run();
  delay(100);
}
