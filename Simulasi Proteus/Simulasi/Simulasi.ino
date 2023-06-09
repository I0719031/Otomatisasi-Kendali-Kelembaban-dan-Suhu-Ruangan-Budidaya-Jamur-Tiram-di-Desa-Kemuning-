/*
 * List Program yg diintegrasi:
 * - Relay          : V
 * - Soil Humadity  : V
 * - DHT22          : V
 * - Encoder Rotary : V
 * - int.ESP & Mifi : V
 * - LCD            : V
 * - HMI            : V
 * - RTC            : V
 * - TestSlnd&Vapor : V
 */

/* HMI Setting Design
 * 1. the design got simplyfied, with static display containing Time and Date, Spraying Schedules, Sensors Detection, and Status (Dry, Normal, Wet).
 * 2. to control the schedules, using switchs to active and deactive the timing.
 * 3. For the time and date,long of spraying time, & spraying timing already preset in program so no need to adjust it again.
 * 4. the spraying timing in detail is at 07.30, 12.00, and 16.30.
 * 5. might gonna give an additional button to manually active spraying mechanism.
  */


/* Serial ESP to Arduino on: 
 * https://create.arduino.cc/projecthub/nidhi17agarwal/uart-communication-between-arduino-uno-and-esp32-1170d5 
 * Shcematic in web, using voltage divider for TX of Arduino, ground connected to both GND.
 * Using generic code like other, just need to code the ESP using Arduino IDE, tutorial in link:
 * https://www.electronicshub.org/esp32-arduino-ide/#:~:text=Type%20the%20code%20in%20Arduino,can%20see%20the%20LED%20Blinking.
 */

 //Kurang status Basah, Normal, Kering 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
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
#define R1  A0 //Pin Relay
#define R2  A1
#define R3  A2
#define R4  A3
#define SSH1  A4 // Pin Soil Humadity Sensor
#define SSH2  A5

#define SP2 10 //Switch for Off/On Scheadule
#define SP3 9 //dibalik karena pemasangan terbalik
#define SP1 8
#define MB1 7 //PushButton for Manual Spraying
#define DHTPIN    3  // Pin digital DHT22
#define DHTTYPE   DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
float SSH1Val = 0;
float SSH2Val = 0;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int nRE = LOW;
int RSP1;
int RSP2;
int RSP3;
int RMB1;

//jadwal penyemprotan, H0,M0,S0 hanya untuk uji coba
int H0= 12;
int M0= 0;
int S0=10; 
int H1= 7; 
int M1= 0;
int S1= 0;
int H2= 14;
int M2= 0;
int S2= 0;
int H3= 17;
int M3= 0;
int S3= 0;

char P1[]= "07:00";
char P2[]= "14:00";
char P3[]= "17:00";

//Initial Alarm 
int Am0 = 0;
int Am1 = 0;
int Am2 = 0;
int Am3 = 0;
int dlyB = 0;

void setup() {
  pinMode(R1,OUTPUT);
  pinMode(R2,OUTPUT);
  pinMode(R3,OUTPUT);
  pinMode(R4,OUTPUT);
  pinMode(SP1,INPUT_PULLUP);
  pinMode(SP2,INPUT_PULLUP);
  pinMode(SP3,INPUT_PULLUP);
  pinMode(MB1,INPUT_PULLUP);
  digitalWrite(R1,HIGH);
  digitalWrite(R2,HIGH);
  digitalWrite(R3,HIGH);
  digitalWrite(R4,HIGH);
  // Setup Serial connection
  Serial.begin(115200);
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
  
void loop() 
{  
  //Sensor DHT22
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

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

  //LCD Loop
  //Jam
  lcd.setCursor(0,0); //mulai dari 0,0
  lcd.print(rtc.getTimeStr());
  //Tanggal
  lcd.setCursor(10,0); 
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
  lcd.setCursor(0,2);
  lcd.print(SSH1Val);
  lcd.setCursor(7,2);
  lcd.print(t);
  lcd.setCursor(12,2);
  lcd.print(F(" C"));
  lcd.setCursor(15,2);
  lcd.print(SSH2Val);
  /*
  lcd.setCursor(0,3);
  lcd.print("B/N/K");
  //ganti status sesuai kalkulasi
  lcd.setCursor(15,3);
  lcd.print("B/N/K");
  //ganti status sesuai kalkulasi
  */
  
  //Sensor Soil Humadity
  SSH1Val = SSH1Val + analogRead(SSH1);
  SSH2Val = SSH2Val + analogRead(SSH2);
  SSH1Val = SSH1Val/100.0;
  SSH2Val = SSH2Val/100.0;
  Serial.print("Val. Sensor Soil 1: ");
  Serial.println(SSH1Val);
  Serial.print("Val. Sensor Soil 2: ");
  Serial.println(SSH2Val);
 //Sensor 1
  if (SSH1Val >= 8)
  {
    lcd.setCursor(0,3);
    lcd.print("KERING");
  }

  if (SSH1Val < 8 && SSH1Val > 4)
  {
    lcd.setCursor(0,3);
    lcd.print("NORMAL");
  }

  if (SSH1Val < 4)
  {
    lcd.setCursor(0,3);
    lcd.print("BASAH ");
  }
 
 //Sensor 2
  if (SSH2Val >= 8)
  {
    lcd.setCursor(14,3);
    lcd.print("KERING");
  }

  if (SSH2Val < 8 && SSH1Val > 4)
  {
    lcd.setCursor(14,3);
    lcd.print("NORMAL");
  }

  if (SSH2Val < 4)
  {
    lcd.setCursor(14,3);
    lcd.print("BASAH ");
  }
 
  //RTC Program
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");
   
  // Send time
  Serial.println(rtc.getTimeStr());
  tt= rtc.getTime();
  
  RSP1= analogRead(SP1);
  RSP3= analogRead(SP2);
  RSP2= analogRead(SP3);
  RMB1= analogRead(MB1);
    
  //Alarm & Switch Setting
  //Switch 1
  if (RSP1 > 1022)
    {
     Serial.println("SP1 Active");
     lcd.setCursor(1,1);
     lcd.print(P1);
     Serial.println("SP1 Active");
     if (tt.hour==H1 && tt.min==M1 && tt.sec==S1)
    {
      Am1 = 2;
      Serial.println("Alarm 1 triggered");
      
      }
    if (Am1 > 0)
    {
      //Spraying mechanism here
      spray();
      Am1 = 0;
    }
    else 
    {
      Serial.println("Alarm 1 Standy");
    }
    }   
  if (RSP1 < 1022)
  {
    Serial.println("SP1 Deactive");
    lcd.setCursor(1,1);
    lcd.print("Off  ");
  }
  //Switch 2
  if (RSP2 > 1022)
    {
     Serial.println("SP2 Active");
     lcd.setCursor(8,1);
     lcd.print(P2);
     if (tt.hour==H2 && tt.min==M2 && tt.sec==S2)
    {
      Am2 = 2;
      Serial.println("Alarm 1 triggered");
      
      }
    if (Am2 > 0)
    {
      //Spraying mechanism here
      spray();
      Am2 = 0;
    }
    else 
    {
      Serial.println("Alarm 2 Standy");
    }
    }   
  if (RSP2 < 1022)
  {
    Serial.println("SP2 Deactive");
    lcd.setCursor(8,1);
    lcd.print("Off  ");
  }
  //Switch 3
  if (RSP3 > 1022)
    {
     Serial.println("SP3 Active");
     lcd.setCursor(15,1);
     lcd.print(P3);
     if (tt.hour==H3 && tt.min==M3 && tt.sec==S3)
    {
      Am3 = 2;
      Serial.println("Alarm 3 triggered");
      
      }
    if (Am3 > 0)
    {
      //Spraying mechanism here
      spray();
      Am3 = 0;
    }
    else 
    {
      Serial.println("Alarm 3 Standy");
    }
    }   
  if (RSP3 < 1022)
  {
     Serial.println("SP3 Deactive");
     lcd.setCursor(15,1);
     lcd.print("Off  ");
  }

  if (RMB1 >= 1023)
    {
     Serial.println("MB1 Delay, 3 Seconds");
     Serial.println(RMB1);
     dlyB= dlyB + 1;
     
    }   
  if (RMB1 < 1022)
  {
    Serial.println("MB1 Deactive");
    Serial.println(RMB1);
  }
 if (RMB1 >= 1023 && dlyB >= 3)
    {
     Serial.println("MB1 Active");
     spray();
     dlyB=0;
    }
 if (RMB1 <= 1022 && dlyB >= 1)
    {
     Serial.println("MB1 Reset"); //to avoid accidental manual spray mechanism
     dlyB=0;
    }   

  delay(1000);
}

  void spray() 
  {
    digitalWrite(A0,LOW);
    delay(120000);
    
    digitalWrite(A0,HIGH);
  }
