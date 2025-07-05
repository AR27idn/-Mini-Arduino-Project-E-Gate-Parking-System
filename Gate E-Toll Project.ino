#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER_PIN 8
#define SERVO_PIN 3

MFRC522 rfid(SS_PIN, RST_PIN);
Servo gateServo;

String knownUIDs[] = {
  "F58B3F2",
  "25EE3C2",
  "35F1A2D"
};

String names[] = {
  "Dinda Gea",
  "Arfy",
  "Cindy"
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(BUZZER_PIN, OUTPUT);
  gateServo.attach(SERVO_PIN);
  gateServo.write(0);
  Serial.println("READY");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  String uidStr = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidStr += String(rfid.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();

  int index = getUIDIndex(uidStr);
  if (index >= 0) {
    tone(BUZZER_PIN, 1200, 300);
    gateServo.write(90);
    delay(3000);
    gateServo.write(0);

    // Kirim data ke Python via serial
    Serial.print(uidStr);
    Serial.print(",");
    Serial.print(names[index]);
    Serial.print(",");
    Serial.println("AKSES DIBERIKAN");

  } else {
    tone(BUZZER_PIN, 300, 1000);
    Serial.print(uidStr);
    Serial.print(",");
    Serial.print("TIDAK DIKENAL");
    Serial.print(",");
    Serial.println("DITOLAK");
  }

  delay(2000);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

int getUIDIndex(String uid) {
  for (int i = 0; i < sizeof(knownUIDs) / sizeof(knownUIDs[0]); i++) {
    if (uid == knownUIDs[i]) return i;
  }
  return -1;
}
