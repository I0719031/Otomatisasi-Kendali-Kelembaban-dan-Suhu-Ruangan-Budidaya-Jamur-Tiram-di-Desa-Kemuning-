/*
 * BISMILLAH PROGRAM MONITORING JAMUR IOT
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
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
Time tt;
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "DHT.h"
#include <DHT_U.h>
#define pompa1  3 //Relay untuk Pompa

#define soil1  A0 // Pin Soil Humadity Sensor
#define soil2  A1 // Pin Soil Humadity Sensor


#define DHTPIN    2  // Pin digital DHT22
#define DHTTYPE   DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

float soil1Val = 0;
float soil2Val = 0;
float h;
float t;
float f;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int nRE = LOW;
int nilaibutton1;
int nilaibutton2;
int nilaibutton3;
int nilaibutton4;

//jadwal penyemprotan, H0,M0,S0 hanya untuk uji coba
int H0 = 12;
int M0 = 0;
int S0 = 10;
int H1 = 7;
int M1 = 0;
int S1 = 0;
int H2 = 14;
int M2 = 0;
int S2 = 0;
int H3 = 17;
int M3 = 0;
int S3 = 0;

char P1[] = "07:00";
char P2[] = "14:00";
char P3[] = "17:00";

//Initial Alarm
int Am0 = 0;
int Am1 = 0;
int Am2 = 0;
int Am3 = 0;
int dlyB = 0;

void setup() {
  pinMode(pompa1, OUTPUT);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  digitalWrite(pompa1, LOW);
  // Setup Serial connection
  Serial.begin(9600);
  sensor_t sensor;
  delayMS = sensor.min_delay / 1000;
  dht.begin();
  // Initialize the rtc object
  rtc.begin();
  // Uncomment to set time
  /*
    rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to DAYS
    rtc.setTime(16, 53, 0);     // Set the time exp: 12:00:00 (24hr format)
    rtc.setDate(18, 3, 2022);   // Set the date to Month, date, Year
  */
  //LCD Setup
  lcd.init();                      // initialize the lcd
  lcd.init();
  lcd.backlight();
}

//Pembacaan sensor DHT22
void dht22() {
  //Sensor DHT22
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
  }

}

//Pembacaan Sensor Kelembaban TANAH
void sensorsoil() {
  soil1Val = analogRead(soil1) * 0.0981;
  soil2Val = analogRead(soil2) * 0.0981;
  //soil1Val = soil1Val / 100.0;
  //soil2Val = soil2Val / 100.0;
  if (soil1Val > 0) {
    Serial.print("Val. Sensor Soil 1: ");
    Serial.println(soil1Val);
  }
  if (soil2Val > 0) {
    Serial.print("Val. Sensor Soil 2: ");
    Serial.println(soil2Val);
  }
}

//Program Layar LCD
void layar() {
  //LCD Loop Jam
  lcd.setCursor(0, 0); //mulai dari 0,0
  lcd.print(rtc.getTimeStr());
  //Tanggal
  lcd.setCursor(10, 0);
  lcd.print(rtc.getDateStr());
  //Status Pemnyemptrotan
  /*
    lcd.setCursor(1,1);
    lcd.print(P1);
    lcd.setCursor(8,1);
    lcd.print(P2);
    lcd.setCursor(15,1);
    lcd.print(P3);
  */
  //Kelembaban & Suhu
  lcd.setCursor(0, 2);
  lcd.print(soil1Val);
  lcd.setCursor(7, 2);
  lcd.print(t);
  lcd.setCursor(12, 2);
  lcd.print(F(" C"));
  lcd.setCursor(15, 2);
  lcd.print(soil2Val);
  /*
    lcd.setCursor(0,3);
    lcd.print("B/N/K");
    //ganti status sesuai kalkulasi
    lcd.setCursor(15,3);
    lcd.print("B/N/K");
    //ganti status sesuai kalkulasi
  */

  //Sensor 1
  if (soil1Val >= 8)
  {
    lcd.setCursor(0, 3);
    lcd.print("KERING");
  }

  if (soil1Val < 8 && soil1Val > 4)
  {
    lcd.setCursor(0, 3);
    lcd.print("NORMAL");
  }

  if (soil1Val < 4)
  {
    lcd.setCursor(0, 3);
    lcd.print("BASAH ");
  }

  //Sensor 2
  if (soil2Val >= 8)
  {
    lcd.setCursor(14, 3);
    lcd.print("KERING");
  }

  if (soil2Val < 8 && soil1Val > 4)
  {
    lcd.setCursor(14, 3);
    lcd.print("NORMAL");
  }

  if (soil2Val < 4)
  {
    lcd.setCursor(14, 3);
    lcd.print("BASAH ");
  }
}

//Scanning Push Button Untuk Tombol Mode
void scan() {
  nilaibutton1 = digitalRead(4);  // read input value
  nilaibutton2 = digitalRead(5);  // read input value
  nilaibutton3 = digitalRead(6);  // read input value

  if (nilaibutton1 == 1) {
    Serial.println("Mode Manual Off");
    lcd.setCursor(1, 1);
    lcd.print("OFF   ");
  }
  if (nilaibutton1 == 0) {
    manual(); // Memanggil Mode Manual
    Serial.println("Mode Manual On");
    lcd.setCursor(1, 1);
    lcd.print("MANUAL");
  }
  if (nilaibutton2 == 1) {
    Serial.println("Mode Hujan Off");
    lcd.setCursor(8, 1);
    lcd.print("OFF  ");
  }
  if (nilaibutton2 == 0) {
    Serial.println("Mode Hujan On");
    lcd.setCursor(8, 1);
    lcd.print("HUJAN");
  }
  if (nilaibutton3 == 1) {
    Serial.println("Mode KEMARAU OFF");
    lcd.setCursor(15, 1);
    lcd.print("OFF  ");
  }
  if (nilaibutton3 == 0) {
    Serial.println("Mode KEMARAU ON");
    lcd.setCursor(15, 1);
    lcd.print("PANAS");
  }
  delay(500);


}

//Membuat Mode manual untuk dijalankan secara manual penyemprotan oleh Petani
void manual() {
  nilaibutton4 = digitalRead(7);  // read input value
  if (nilaibutton4 == 1) {
    digitalWrite(pompa1, LOW); //Pompa dimatikan secara manual oleh Petani
  }
  if (nilaibutton4 == 0) {
    digitalWrite(pompa1, HIGH); //Pompa dinyalakan secara manual oleh Petani
    Serial.println("Pompa Menyala Manual");
  }
}

// Program Looping Utama
void loop()
{
  scan(); //mode yang dipilih
  dht22();
  sensorsoil();
  layar();
 
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


}

// Program Pompa Spray
void spray()
{
  digitalWrite(pompa1, LOW);
  delay(120000);

  digitalWrite(pompa1, HIGH);
}
