//Config for blynk API to work
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2xjj94ysA"
#define BLYNK_TEMPLATE_NAME "WaterMonitor"
#define BLYNK_AUTH_TOKEN "ueLwCMFhmr9bOrtIrHoNr3pKGPflc0jS"

//Libs
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <BlynkSimpleEsp32.h>
#include <Blynk.h>

#define pinoDHT 23
#define pinoLDR 34

int leituraLDR;
float temperatura;
float umidade;

//Initialize objects
DHT dht(pinoDHT, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

void initLCD(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 1);
  lcd.clear();
}

void collectAndSendData(){
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  leituraLDR = analogRead(pinoLDR);
  Serial.println(leituraLDR);
  if (isnan(umidade)||isnan(temperatura)){
    lcd.setCursor(0,1);
    lcd.print("FALHA DHT");
  }
  else {
    Blynk.virtualWrite(V0, temperatura);
    Blynk.virtualWrite(V1, umidade);
    Blynk.virtualWrite(V2, leituraLDR);
    lcd.setCursor(0,1);
    lcd.print("Temp: ");
    lcd.print(temperatura);
    lcd.print((char)223);
    lcd.print("C ");
    lcd.setCursor(0,0);
    lcd.print("U: ");
    lcd.print(umidade);
    lcd.print("% ");
    lcd.print("L:");
    lcd.print(leituraLDR);
    lcd.print(" ");
  }
}

void setup() {
  initLCD();
  dht.begin();
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, "Pdr", "blme5011");
  timer.setInterval(2000L, collectAndSendData);
}

void loop() {
  Blynk.run();
  timer.run();
}
