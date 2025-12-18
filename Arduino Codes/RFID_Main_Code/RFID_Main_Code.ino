#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define buzzer 2
#define servoPin 3
#define ledPin 4  

String authorizedCards[] = {
  "53 8D 8F 14",   
  "45 9B B1 D3"    
};

String cardNames[] = {
  "Ahmed",       
  "Mohamed"      
};

const byte totalCards = 2;
byte lock = 0;

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);

  servo.attach(servoPin);
  servo.write(0);   

  lcd.init();
  lcd.backlight();

  SPI.begin();
  rfid.PCD_Init();

  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 
}

void loop() {

  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(1, 1);
  lcd.print("Put your card");

  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning");

  String ID = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    lcd.print(".");
    ID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    ID.concat(String(rfid.uid.uidByte[i], HEX));
    delay(300);
  }

  ID.toUpperCase();
  String scannedID = ID.substring(1);

  bool authorized = false;
  int userIndex = -1;

  for (byte i = 0; i < totalCards; i++) {
    if (scannedID == authorizedCards[i]) {
      authorized = true;
      userIndex = i;
      break;
    }
  }

  if (authorized && lock == 0) {

    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome");
    lcd.setCursor(0, 1);
    lcd.print(cardNames[userIndex]);

    digitalWrite(ledPin, HIGH);
    servo.write(180);

    delay(2000);
    lcd.clear();

    lock = 1;
  }

  else if (authorized && lock == 1) {

    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Good Bye");
    lcd.setCursor(0, 1);
    lcd.print(cardNames[userIndex]);

    digitalWrite(ledPin, LOW);
    servo.write(0);

    delay(2000);
    lcd.clear();

    lock = 0;
  }

else {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong card!");

  for (byte i = 0; i < 5; i++) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(150);

    digitalWrite(buzzer, LOW);
    digitalWrite(ledPin, LOW);
    delay(150);
  }

  delay(1000);
  lcd.clear();
 }
}
