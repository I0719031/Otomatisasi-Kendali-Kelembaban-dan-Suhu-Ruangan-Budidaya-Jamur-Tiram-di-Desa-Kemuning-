unsigned long hitungan_milis;  //variable yang nantinya kita gunakan untuk menyimpan milis
unsigned long milis_sekarang;
const unsigned long nilai = 3000;  //nilai yang akan kita jadikan patokan 1000 = 1 detik
unsigned long hitungan_milis1;  //variable yang nantinya kita gunakan untuk menyimpan milis
unsigned long milis_sekarang1;
const unsigned long nilai1 = 4000;  //nilai yang akan kita jadikan patokan 1000 = 1 detik
unsigned long hitungan_milis2;  //variable yang nantinya kita gunakan untuk menyimpan milis
unsigned long milis_sekarang2;
const unsigned long nilai2 = 5000;  //nilai yang akan kita jadikan patokan 1000 = 1 detik



const byte ledPin = 13;    //pin LED yang akan kalian gunakan di Arduino

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int nilaibutton1;
int nilaibutton2;
int nilaibutton3;
int nilaibutton4;
void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(9, INPUT);
  hitungan_milis = 0;  //menyimpan milidetik terakhir dari loop
  //LCD Setup
  lcd.init();                      // initialize the lcd
  lcd.init();
  lcd.backlight();
}

void loop()
{
  nilaibutton1 = digitalRead(6);  // read input value
  nilaibutton2 = digitalRead(7);  // read input value
  nilaibutton3 = digitalRead(9);  // read input value

  milis_sekarang = millis();  //mengambil waktu yang berjalan dan menyimpan di variabel milis_sekarang
  milis_sekarang1 = millis();  //mengambil waktu yang berjalan dan menyimpan di variabel milis_sekarang
  milis_sekarang2 = millis();  //mengambil waktu yang berjalan dan menyimpan di variabel milis_sekarang
  Serial.println(milis_sekarang);
  if (nilaibutton1 == 0) {
    if (milis_sekarang - hitungan_milis >= nilai)  //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 1000
    {
      Serial.println("Mode Manual Off");
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("MODE");
      lcd.setCursor(4, 1);
      lcd.print("MANUAL OFF");
      hitungan_milis = milis_sekarang;  //waktu yang disimpan dari saat LED hidup / mati
    }
  }

  if (nilaibutton2 == 0) {
    if (milis_sekarang1 - hitungan_milis1 >= nilai1)  //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 1000
    {
      Serial.println("Mode Manual On");
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("MODE");
      lcd.setCursor(4, 1);
      lcd.print("MANUAL ON");
      hitungan_milis1 = milis_sekarang1;  //waktu yang disimpan dari saat LED hidup / mati
    }
  }

  if (nilaibutton3 == 0) {
    if (milis_sekarang2 - hitungan_milis2 >= nilai2)  //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 1000
    {
      Serial.println("Mode Hujan Off");
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print("MODE");
      lcd.setCursor(4, 1);
      lcd.print("HUJAN OFF");
      hitungan_milis2 = milis_sekarang2;  //waktu yang disimpan dari saat LED hidup / mati
    }
  }



  /*
    milis_sekarang = millis();  //mengambil waktu yang berjalan dan menyimpan di variabel milis_sekarang
    Serial.println(milis_sekarang);
    if (milis_sekarang - hitungan_milis >= nilai)  //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 1000
    {
    digitalWrite(ledPin, !digitalRead(ledPin));  //LED akan menyala dan mati secara bergantian
    hitungan_milis = milis_sekarang;  //waktu yang disimpan dari saat LED hidup / mati
    }
  */
}
