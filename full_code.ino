#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include "DFRobotDFPlayerMini.h"

int relayPin = 26;
int buzzerPin = 14;
HardwareSerial Myserial(0); // UART1 (0), UART2 (1), UART3 (2).
DFRobotDFPlayerMini mp3; // Membuat variabel "myDFPlayer"
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
 

void setup()
{
    lcd.begin(); 
    Serial.begin(115200);
    finger.begin(57600);
    pinMode(relayPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(relayPin, HIGH);
    lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print("DoorLock finger");
    Serial.println("Fingerprint Door Lock");
    delay(2000);
    Myserial.begin(9600, SERIAL_8N1, 4, 2);  //kecepatan, jenis komunikasi, pin RX, TX
    mp3.begin(Myserial);
}
 
void loop()                // run over and over again
{
    getFingerprintIDez();
    delay(50);            //don't need to run this at full speed.
}
 
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() 
 {
  uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)  
    {
    lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print("Fingerprint Siap");
    Serial.println("Waiting For Valid Finger");
    return -1;
  }
 
 p = finger.image2Tz();
 if (p != FINGERPRINT_OK)  
 {
   //======================Error
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print("   Coba Lagi    ");
    Serial.println("Messy Image Try Again");
    mp3.play(3); 
    delay(2000);
    lcd.clear();
    return -1;
    }
 
 p = finger.fingerFastSearch();
 if (p != FINGERPRINT_OK)  {
   //==============================Salah
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print("Tidak Terdaftar ");
    Serial.println("Not Valid Finger");
    mp3.play(3); 
    delay(2000);
    lcd.clear();
    return -1;
  }
 
    // ========================Benar
    digitalWrite(relayPin, LOW);
    mp3.play(1); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print(" Selamat Datang ");
    Serial.println("Door Unlocked Welcome");
    delay(3000);
    digitalWrite(relayPin, HIGH);
    Serial.println("The door is locked automatically");
    lcd.clear();
    return finger.fingerID;
 }
