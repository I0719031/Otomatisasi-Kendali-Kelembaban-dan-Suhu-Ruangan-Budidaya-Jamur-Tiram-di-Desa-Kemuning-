/*
   BISMILLAH PROGRAM MONITORING JAMUR IOT
  /* Serial ESP to Arduino on:
   https://create.arduino.cc/projecthub/nidhi17agarwal/uart-communication-between-arduino-uno-and-esp32-1170d5
   Shcematic in web, using voltage divider for TX of Arduino, ground connected to both GND.
   Using generic code like other, just need to code the ESP using Arduino IDE, tutorial in link:
   https://www.electronicshub.org/esp32-arduino-ide/#:~:text=Type%20the%20code%20in%20Arduino,can%20see%20the%20LED%20Blinking.
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
// Arduino Mega:
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//Pembacaan DHT22
#include "DHT.h"
#define DHTPIN 4    // DHT PIN 2
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "DHT.h"
#include <DHT_U.h>
#define pompa1  5 //Relay untuk Pompa


float h;
float t;

byte nilaibutton1;
byte nilaibutton2;
byte nilaibutton3;
byte nilaibutton4;

//Setingan RTC, jam, dan alarm
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
Time tt;
byte jam, menit, detik;
byte set_jam1 = 7;     //Set alarm jam Pagi
byte set_menit1 = 00;   //Set alarm menit Pagi
byte set_jam2 = 14;     //Set alarm jam Siang
byte set_menit2 = 00;   //Set alarm menit Siang
byte set_jam3 = 17;     //Set alarm jam Malam
byte set_menit3 = 00;   //Set alarm menit Malam


void setup() {
  pinMode(pompa1, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(9, INPUT);
  digitalWrite(pompa1, HIGH);
  // Setup Serial connection
  Serial.begin(9600);

  dht.begin();
  // Initialize the rtc object
  rtc.begin();
  // Uncomment to set time

  rtc.setDOW(5);     // Set Day-of-Week to DAYS(0=senin)
  rtc.setTime(22, 30, 0);     // Set the time exp: 12:00:00 (24hr format)
  rtc.setDate(24, 12, 2022);   // Set the date to Month, date, Year

  //LCD Setup
  lcd.init();                      // initialize the lcd
  lcd.init();
  lcd.backlight();
}

//Pembacaan sensor DHT22
void dht22() {
  delay(500);
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
}

void siram1() {
  if (set_jam1 == jam) {             //Jika jam sama dengan alarm, maka
    if (set_menit1 == menit) {       //Jika menit sama dengan alarm, maka
      spray();
      Serial.println("alarm Pagi");
    }
  }
}

void siram2() {
  if (set_jam2 == jam) {             //Jika jam sama dengan alarm, maka
    if (set_menit2 == menit) {       //Jika menit sama dengan alarm, maka
      spray();
      Serial.println("alarm Siang");
    }
  }
}

void siram3() {
  if (set_jam3 == jam) {             //Jika jam sama dengan alarm, maka
    if (set_menit3 == menit) {       //Jika menit sama dengan alarm, maka
      spray();
      Serial.println("alarm Sore");
    }
  }
}

//Scanning Push Button Untuk Tombol Mode
void scan() {
  nilaibutton1 = digitalRead(6);  // read input value
  nilaibutton2 = digitalRead(7);  // read input value
  nilaibutton3 = digitalRead(8);  // read input value

  if (nilaibutton1 == 1) {
    digitalWrite(pompa1, HIGH);
    Serial.println("Mode Manual Off");
    lcd.setCursor(1, 2);
    lcd.print("OFF   ");
  }
  if (nilaibutton1 == 0) {
    Serial.println("Mode Manual On");
    digitalWrite(pompa1, LOW);
    lcd.setCursor(1, 2);
    lcd.print("MANUAL");
  }

  if (nilaibutton2 == 1) {
    Serial.println("Mode Hujan Off");
    lcd.setCursor(8, 2);
    lcd.print("OFF  ");
  }
  if (nilaibutton2 == 0) {
    Serial.println("Mode Hujan On");
    lcd.setCursor(8, 2);
    lcd.print("HUJAN");
    siram1();
    siram2();
    siram3();
  }
  if (nilaibutton3 == 1) {
    lcd.setCursor(15, 2);
    lcd.print("OFF  ");
    Serial.println("Mode KEMARAU OFF");
  }
  if (nilaibutton3 == 0) {
    lcd.setCursor(15, 2);
    lcd.print("PANAS");
    Serial.println("Mode KEMARAU ON");
    siram1();
    siram2();
    siram3();
  }
}

void RTC() {
  //RTC Program
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");

  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  tt = rtc.getTime();
  jam = tt.hour;
  menit = tt.min;
  detik = tt.sec;
}

// Program Looping Utama
void loop()
{
  dht22();
  scan(); //mode yang dipilih
  RTC();
  layar();
  if (t > 37) {
    digitalWrite(pompa1, LOW);
  }
}

void layar() {
  //Tulisan Judul
  lcd.setCursor(4,0);
  lcd.print("MBKM UNS 2022");
  //LCD Loop Jam
  lcd.setCursor(0, 3); //mulai dari 0,0
  lcd.print(rtc.getTimeStr());
  //Tanggal
  lcd.setCursor(10, 3);
  lcd.print(rtc.getDateStr());
  //Kelembaban & Suhu
  lcd.setCursor(0, 1);
  lcd.print("K :");
  lcd.setCursor(3, 1);
  lcd.print(h);
  lcd.setCursor(7, 1);
  lcd.print(" %");
  lcd.setCursor(11, 1);
  lcd.print("S :");
  lcd.setCursor(14, 1);
  lcd.print(t);
  lcd.setCursor(18, 1);
  lcd.print(" C");
}


// Program Pompa Spray
void spray()
{
  digitalWrite(pompa1, LOW);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("POMPA");
  lcd.setCursor(4, 1);
  lcd.print("MENYALA");
  delay(60000);
  digitalWrite(pompa1, HIGH);
}
