//Config for blynk API to work
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_HERE"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_HERE"

//Config vars for the TDS Sensor
#define VREF 5.0
#define SCOUNT 30
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
String tdsValue;

//Libs
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <BlynkSimpleEsp32.h>
#include <Blynk.h>

#define pinSensor 15
#define pinoDHT 23
#define pinoLDR 34
#define pinoLED 13
#define pinoLED2 14
#define RXp2 16
#define TXp2 17

int leituraLDR;
float temperatura;
float umidade;

//[Temp],[pH],[TDS]
int posicao_separadorTDS, posicao_separadorPH;

//Initialize objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;


void initLCD(){
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void collectAndSendData(){
  tdsValue = Serial2.readStringUntil('\n');
  int contador_separadores = 0;
  tdsValue.trim();
  if (tdsValue == ""){
    lcd.setCursor(0,1);
    lcd.print("FALHA ARDUINO");
  }
  else {
    //Serial.println(tdsValue.toInt());
    for (int i=0; i<tdsValue.length(); i++){
      if (contador_separadores == 0 && tdsValue[i]==','){
        posicao_separadorTDS = i;
        contador_separadores++;
      }
      else if (contador_separadores == 1 && tdsValue[i]==',') {
        posicao_separadorPH = i;
        contador_separadores++;
      }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TDS:");
    for (int i=posicao_separadorPH+1; i<tdsValue.length()-1; i++){
      lcd.print(tdsValue[i]);
    }
    lcd.print("ppm");
    lcd.setCursor(0,1);
    lcd.print("T:");
    for (int i=0; i<posicao_separadorTDS; i++){
      lcd.print(tdsValue[i]);
    }
    lcd.print((char)223);
    lcd.print("C ");
    lcd.print("pH:");
    for (int i=posicao_separadorTDS+1; i<posicao_separadorPH; i++){
      lcd.print(tdsValue[i]);
    }
  }
}

void setup() {
  pinMode(pinoLED, OUTPUT);
  pinMode(pinoLED2, OUTPUT);
  pinMode(pinSensor, INPUT);
  initLCD();
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  //Blynk.begin(BLYNK_AUTH_TOKEN, "Pdr", "blme5011");
  //timer.setInterval(2000L, collectAndSendData);
}

void loop() {
  //Blynk.run();
  //timer.run();
  collectAndSendData();
  delay(2000);
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
