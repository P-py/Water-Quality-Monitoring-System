//Config for blynk API to work
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_HERE"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_HERE"

//Libs
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <BlynkSimpleEsp32.h>
#include <Blynk.h>

#define pinoDHT 23
#define pinoLDR 34
#define pinoLED 13
#define pinoLED2 14

int leituraLDR;
float temperatura;
float umidade;

//Initialize objects
DHT dht(pinoDHT, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;


void initLCD(){
  lcd.init();
  lcd.clear();
}

void collectAndSendData(){
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  leituraLDR = analogRead(pinoLDR);
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
  pinMode(pinoLED, OUTPUT);
  pinMode(pinoLED2, OUTPUT);
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

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V3);
}

BLYNK_WRITE(V3){
  if (param.asInt()==1){
    Serial.println(param.asInt());
    digitalWrite(pinoLED, HIGH);
    digitalWrite(pinoLED2, HIGH);
    lcd.backlight();
  }
  else {
    Serial.println(param.asInt());
    digitalWrite(pinoLED, LOW);
    digitalWrite(pinoLED2, LOW);
    lcd.noBacklight();
  }
}
