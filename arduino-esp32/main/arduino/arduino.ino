#include <OneWire.h>
#include <DallasTemperature.h>

#define pinOneWire 12

#define TdsSensorPin A1
#define VREF 5.0   // analog reference voltage(Volt) of the ADC
#define SCOUNT 30  // sum of sample point

#define pinESP32 8

#define pinPH A0

OneWire oneWire(pinOneWire);
DallasTemperature sensor(&oneWire);
DeviceAddress endereco_temp;

int analogBuffer[SCOUNT];  // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

float calibration = 0;
int phSensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;

int samples = 10;
float adc_resolution = 1024.0;

float phFunc (float voltage){
  return 7 + ((2.5-voltage) / 0.18);
}

void setup() {
  Serial.begin(9600);
  sensor.begin();
  pinMode(TdsSensorPin, INPUT);
  pinMode(pinESP32, OUTPUT);
}

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void temperatureSensor() {
  sensor.requestTemperatures();
  if (!sensor.getAddress(endereco_temp, 0)) {
    Serial.print("NA");
  } else {
    Serial.print(sensor.getTempC(endereco_temp), 1);
  }
}

void phSensor(){
  //Version 1
  //float ph = map(analogRead(pinPH), 0.0, 900.0, 14.0, 0.0);
  //if (ph<7){
  //  ph-=1;
  //}
  //else if (ph>7){
  //  ph+=1;
  //}

  int measurings = 0;
  for (int i=0; i<samples; i++){
    measurings += analogRead(pinPH);
    delay(10);
  }
  float voltage = (5 / adc_resolution) * (measurings/samples);
  float ph = phFunc(voltage);
  
  Serial.print(",");
  Serial.print(ph);


  //Serial.print(ph);
  //for (int i=0; i<10; i++){
  //  buf[i] = analogRead(pinPH);
  //  delay(30);
  //}
  //for (int i=0; i<9; i++){
  //  for (int j=i+1; j<10; j++){
  //    if (buf[i]>buf[j]){
  //      temp = buf[i];
  //      buf[i] = buf[j];
  //      buf[j] = temp;
  //    }
  //  }
  //}
  //avgValue = 0;
  //for (int i=2; i<8; i++){
  //  avgValue += buf[i];
  //}
  //float phValue = (float) ((avgValue*5.0/1024/6)*3.5)+calibration;
  //float phValue = analogRead(pinPH) * 5.0 / 1024.0;
  //phValue *= 1000;
  //Serial.print(",");
  //Serial.print(phValue);
}

void loop() {
  temperatureSensor();
  phSensor();
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);  //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;                                                                                                   // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                                //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient;                                                                                                             //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;  //convert voltage value to tds value
    //Serial.print("EC:");
    //Serial.print(compensationVolatge,2);
    //Serial.print("\t");
    Serial.print(",");
    Serial.print(tdsValue, 0);
    Serial.println("/");
    //leitura_A0 = analogRead(A0);
    //tensao =  (leitura_A0*5.0) / 1024.0;
    //Serial.println(tensao);
    delay(2000);
  }
}