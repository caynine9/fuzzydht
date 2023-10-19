#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN 13

DHT dht(DHTPIN, DHTTYPE);

int getTemperatureLevel(float temperature);
int getHumidityLevel(float humidity);
int applyDataFuzzyRules(float tempLevel, float humLevel);

float humidity, temp;
float previousTemp = 0.0;
float previousHumidity = 0.0;

unsigned long previousMillis = 0;
unsigned long lastResultMillis = 0;
const long interval = 2000;

const int sendData = 1;
const int doNotSendData = 0;

const int ruleCount = 9;

int dataRules[ruleCount] = {
  doNotSendData, sendData, sendData,
  sendData, doNotSendData, sendData,
  sendData, sendData, doNotSendData
};

int getTemperatureLevel (float temperature) {
  if (temperature <= 15) return 0;
  if (temperature <= 25) return 1;
  return 2;
} 

int getHumidityLevel (float humidity) {
  if (humidity <= 30) return 0;
  if (humidity <= 60) return 1;
  return 2;
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  humidity = dht.readHumidity();
  temp = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C");

  Serial.print(" | ");


  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}


void loop() {
  gettemperature();
}

int applyDataFuzzyRules(int tempLevel, int humLevel){
  return dataRules[tempLevel *3 + humLevel];
}

void gettemperature() {
  unsigned long currentMillis = millis();

  int tempLevel = getTemperatureLevel(temp);
  int humLevel = getHumidityLevel(humidity);

  int sendDataDecision = applyDataFuzzyRules(tempLevel, humLevel);

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    humidity = dht.readHumidity();
    temp = dht.readTemperature();

    if (isnan(humidity) || isnan(temp)) {
      Serial.println("Sensor not detected.");
      return;
    }

    float tempChange = abs(temp - previousTemp);
    float humChange = abs(humidity - previousHumidity);

    if (tempChange >= 0.5 || humChange >= 0.5) {
      if (sendDataDecision == sendData) {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C");

      Serial.print(" | ");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      unsigned long elapsedMillis = currentMillis - lastResultMillis;

      unsigned long elapsedSeconds = elapsedMillis/1000;

      unsigned long seconds = elapsedSeconds % 60;
      unsigned long minutes = (elapsedSeconds % 3600)/60;
      unsigned long hours = elapsedSeconds/3600;

      String formattedTime = String(hours) + ":" + String(minutes) + ":" + String(seconds);

      Serial.print("Time since last result: ");
      Serial.print(formattedTime);
      Serial.println(" (HH:mm:ss)");

      lastResultMillis = currentMillis;
        }
    }
    previousTemp = temp;
    previousHumidity = humidity; 
  }
}