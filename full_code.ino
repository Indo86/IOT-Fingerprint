#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h> // Tambahkan library untuk LCD

// Pin RX dan TX yang digunakan untuk ESP32
#define RX_PIN 16  // Sambungkan ke TX2 ESP32
#define TX_PIN 17  // Sambungkan ke RX2 ESP32

// Pin Servo
#define SERVO_PIN 25

HardwareSerial mySerial(2); // Gunakan UART2 (Serial2) ESP32
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C default untuk LCD adalah 0x27

uint8_t id;

void setup() {
  Serial.begin(115200);     // Debug serial untuk monitoring
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);  // Inisialisasi Serial2
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // Inisialisasi LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);

  // Cek koneksi dengan sensor fingerprint
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print("Sensor Ready!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.print("Sensor Error!");
    while (1) {
      delay(1);
    }
  }

  // Inisialisasi Servo
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Posisi awal servo terkunci

  delay(2000);
  showMenu();
}

void loop() {
  if (Serial.available()) {
    char choice = Serial.read(); // Membaca input pilihan dari Serial Monitor
    switch (choice) {
      case '1':
        lcd.clear();
        lcd.print("Enroll Finger");
        Serial.println("Ready to enroll a fingerprint!");
        Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
        id = readnumber();
        if (id == 0) {
          return;
        }
        Serial.print("Enrolling ID #");
        Serial.println(id);
        lcd.clear();
        lcd.print("Enroll ID: ");
        lcd.print(id);
        while (!getFingerprintEnroll()) break;
        break;
      case '2':
        verifyFingerprint();
        break;
      default:
        Serial.println("Pilihan tidak valid. Silakan coba lagi.");
        showMenu();
        break;
    }
  }
}

void verifyFingerprint() {
  Serial.println("\n=== Verifikasi Sidik Jari ===");
  lcd.clear();
  lcd.print("Input Finger...");
  Serial.println("Letakkan jari di sensor...");

  while (finger.getImage() != FINGERPRINT_OK) delay(100);

  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Gagal mengonversi gambar ke template.");
    lcd.clear();
    lcd.print("Login Failed :(");
    showMenu();
    return;
  }

  int id = finger.fingerSearch();
  if (id == FINGERPRINT_OK) {
    Serial.println("Sidik jari cocok dengan ID: " + String(finger.fingerID));
    lcd.clear();
    lcd.print("Access Granted!");
    lcd.setCursor(0, 1);
    lcd.print(":)");
    Serial.println("Confidence Level: " + String(finger.confidence));

    // Aksi: Buka kunci pintu
    Serial.println("Akses diterima! Membuka kunci pintu...");
    myServo.write(90); // Servo ke posisi buka
    delay(5000); // Tunggu 5 detik
    Serial.println("Mengunci pintu kembali.");
    lcd.clear();
    lcd.print("Locking Door...");
    myServo.write(0); // Kunci kembali
  } else {
    Serial.println("Sidik jari tidak ditemukan.");
    lcd.clear();
    lcd.print("Access Denied!");
    lcd.setCursor(0, 1);
    lcd.print(":(");
  }

  delay(3000); // Tampilkan pesan di LCD selama 3 detik
  showMenu();
}

void showMenu() {
  Serial.println("\n=== Menu Fingerprint ===");
  lcd.clear();
  lcd.print("Menu:");
  lcd.setCursor(0, 1);
  lcd.print("1.Enroll 2.Verify");
  Serial.println("1. Enroll Sidik Jari Baru");
  Serial.println("2. Verifikasi Sidik Jari");
  Serial.print("Masukkan pilihan: ");
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
    delay(2000);
  }
  return num;
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    yield();

    p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    yield();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }
