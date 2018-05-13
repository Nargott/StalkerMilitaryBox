#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Bounce.h>
#include <Servo.h>

#define BTN_PIN 4
#define BTN_LED_PIN 5
#define SERVO_PIN 9
#define DISK_PULSE 2 //контакт импульсов номеронабирателя
#define DISK_SHUNT 3 //шунт, контакт сигнала начала ввода

#define MAX_CODE_LENGTH 9

LiquidCrystal_I2C lcd(0x3f, 16, 2);
Servo doorSrv;
Bounce num =  Bounce(DISK_PULSE, 5);
Bounce flag = Bounce(DISK_SHUNT, 5);
Bounce btn = Bounce(BTN_PIN, 5);

char count = 0;

String mainCode = "12345";
String reserveCode = "987654321";
String enteredCode = "";

bool IS_OPENED = false;

void setup() {            
  pinMode(DISK_PULSE, INPUT);
  pinMode(DISK_SHUNT, INPUT);
  pinMode(BTN_PIN, INPUT);
  pinMode(BTN_LED_PIN, OUTPUT);
  lcd.begin();
  lcd.backlight();
  doorSrv.attach(9);

  digitalWrite(BTN_LED_PIN, LOW);
  closeDoor();
  showCode();  
}

void loop() {
  if (IS_OPENED) {
    if (btn.update()) {
     if (btn.read() == HIGH) {
        openDoor();
     }
    }
  }
  if (!IS_OPENED) {
    if ( num.update() ) {
     if (num.read() == HIGH) {
        count++;
     }
    }    
    if (flag.update()) {
       if (flag.read() == HIGH) {
          count=-1;
       } else
          count++;
          if (count == 10) {count=0;}
          if (count > -1) {
            enteredCode += String(int(count));
            showCode();
            if (enteredCode.length() > MAX_CODE_LENGTH) {
              IS_OPENED = false;
              showError();
              //wait a little
              delay(5000); //5 sec
              enteredCode = "";
              showCode();
            }
            
            if ((enteredCode == mainCode) || (enteredCode == reserveCode)) {
              IS_OPENED = true;
              showFine();
            }
        }
     }    
  }
}

void showError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STATUS: CLOSED");
  lcd.setCursor(0, 1);
  lcd.print("CODE IS INVALID!");
}

void showFine() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STATUS: OPENED");
  lcd.setCursor(0, 1);
  lcd.print(enteredCode);

  digitalWrite(BTN_LED_PIN, HIGH);
}

void showCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER CODE:");
  lcd.setCursor(0, 1);
  lcd.print(enteredCode);
}

void openDoor() {
  doorSrv.write(83);
}

void closeDoor() {
  doorSrv.write(165);
}
