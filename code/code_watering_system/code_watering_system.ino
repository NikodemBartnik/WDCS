/*
Project by Nikodem Bartnik
Simple watering system for a garden or greenhouse with additional data logging and sensors.

Libraries you will need to Arduino IDE, in () version that I used in my project:
 - DallasTemperature by Miles Burton, Tim Newsome, Guil Barros, Rob, Tillaart (3.9.0)
 - 
 You can use neewer library version if it works
*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>

//3 temperature sensors are connected to one pin

//we don't need to read the temperature really fast so we can use the highest precision
#define TEMPERATURE_PRECISION 12
#define NUMBER_OF_READINGS_TO_AVERAGE 20
#define LOW_MOISTURE_LEVEL_1 700
#define LOW_MOISTURE_LEVEL_2 700
#define WATERING_TIME_SECONDS_1 300
#define WATERING_TIME_SECONDS_2 60
#define DATA_READINGS_WHILE_WATERING 50
#define MAX_NO_WATERING_TIME 86400000 //miliseconds = 24 hours
#define MIN_NO_WATERING_TIME 43200000 //miliseconds = 12 hours

#define PIN_TEMPERATURE_SENSOR 2
#define PIN_SD_CS 10
#define PIN_WATER_LEVEL_SENSOR_1 5
#define PIN_WATER_LEVEL_SENSOR_2 6
#define PIN_PUMP_1 7
#define PIN_PUMP_2 8
#define PIN_DATA_SAVING_LED 9
#define PIN_MOISTURE_SENSOR_1 A0
#define PIN_MOISTURE_SENSOR_2 A1
#define PIN_RAIN_SENSOR A2
#define PIN_SUN_SENSOR A3
#define PIN_BATTERY_VOLTAGE A4
#define PIN_PV_VOLTAGE A5

//at the end of setup function we choose the name of a file where we will store our data
String sd_file_name = "";
int watering_counter_1 = 0;
int watering_counter_2 = 0;
long int last_watering_1 = 0;
long int last_watering_2 = 0;


OneWire oneWire(PIN_TEMPERATURE_SENSOR);
DallasTemperature sensors(&oneWire);

//addresses of each temperature sensor, checked with a program from examples
DeviceAddress temp_ground1 = { 0x28, 0xC4, 0xFB, 0x3E, 0x3A, 0x19, 0x1, 0x62 };
DeviceAddress temp_ground2 = { 0x28, 0x95, 0xAB, 0x37, 0x3A, 0x19, 0x1, 0x7D };
DeviceAddress temp_air = { 0x28, 0x6, 0x4, 0x26, 0x3A, 0x19, 0x1, 0x7E };

void setup(void) {
  Serial.begin(9600);
  sensors.begin();
  //setting the previously definied resolution
  sensors.setResolution(temp_ground1, TEMPERATURE_PRECISION);
  sensors.setResolution(temp_ground2, TEMPERATURE_PRECISION);
  sensors.setResolution(temp_air, TEMPERATURE_PRECISION);

  pinMode(PIN_WATER_LEVEL_SENSOR_1, INPUT_PULLUP);
  pinMode(PIN_WATER_LEVEL_SENSOR_2, INPUT_PULLUP);
  pinMode(PIN_PUMP_1, OUTPUT);
  pinMode(PIN_PUMP_2, OUTPUT);
  pinMode(PIN_DATA_SAVING_LED, OUTPUT);

  digitalWrite(PIN_PUMP_1, HIGH);
  digitalWrite(PIN_PUMP_2, HIGH);
  digitalWrite(PIN_DATA_SAVING_LED, LOW);

  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("Card failed, or not present");
    errorAndHold();
  }
  //finding a name for a new file
  checkAndCreateNewFile();
  for(int i = 0; i < 10; i++){
    digitalWrite(PIN_DATA_SAVING_LED, !digitalRead(PIN_DATA_SAVING_LED));
    delay(200);
  }
  Serial.println("Program by Nikodem Bartnik V0.6");
  if(getWaterLevel(1) == 0){
  Serial.println("Pump test...");
  switchPump(1,1);
  delay(5000);
  switchPump(1,0);
  Serial.println("Test finished...");
  }
}

//returns the temperature from each DS18B20 sensor
float getTemperature(DeviceAddress deviceAddress) {
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
    return -1;
  }
  return tempC;
  //just to indicate that we went sucessfully through setup and we are ready to start the program
}

void checkAndCreateNewFile() {
  //checking if there exists file gd_n.txt for n from 0 to 1000
  //just to make sure that after each restart there will be a separete file with data
  for (int i = 0; i < 1000; i++) {
    if (!SD.exists("gd" + String(i) + ".txt")) {
      //if the file doesn't exists we save the name as sd_file_name
      sd_file_name = "gd" + String(i) + ".txt";
      Serial.println("File name: " + sd_file_name);
      break;
    }
  }
}

//saving all the values in the file
void saveGeneralDataToSD(long int mil, float t_g1, float t_g2, float t_air, int m_g1, int m_g2, int w_l1, int w_l2, float bat_voltage, float pv_voltage, int sun, int rain) {
  digitalWrite(PIN_DATA_SAVING_LED, HIGH);
  File datafile = SD.open(sd_file_name, FILE_WRITE);
  if (datafile) {
    String data = String(mil) + ", " + String(t_g1) + ", " + String(t_g2) + ", " + String(t_air) + ", " + String(m_g1) + ", " + String(m_g2) + ", " + String(w_l1) + ", " + String(w_l2) + ", " + String(bat_voltage) + ", " + String(pv_voltage) + ", " + String(sun) + ", " + String(rain);
    datafile.println(data);
    datafile.close();
    digitalWrite(PIN_DATA_SAVING_LED, LOW);
    Serial.println(data);
  } else {
    Serial.println("File opening error");
  }
}

void saveWateringDataToSD(int watering_counter, int pump, long int mil, float t_g1, float t_g2, float t_air, int m_g1, int m_g2, int w_l1, int w_l2, float bat_voltage, float pv_voltage, int sun, int rain) {
  digitalWrite(PIN_DATA_SAVING_LED, HIGH);
  File datafile;
  if (pump == 1) {
    datafile = SD.open("p1.txt", FILE_WRITE);
  } else {
    datafile = SD.open("p2.txt", FILE_WRITE);
  }
  String data = String(watering_counter) + ", " + String(mil) + ", " + String(t_g1) + ", " + String(t_g2) + ", " + String(t_air) + ", " + String(m_g1) + ", " + String(m_g2) + ", " + String(w_l1) + ", " + String(w_l2) + ", " + String(bat_voltage) + ", " + String(pv_voltage) + ", " + String(sun) + ", " + String(rain);
  datafile.println(data);
  datafile.close();
  digitalWrite(PIN_DATA_SAVING_LED, LOW);
}

int getMoistureSensorData(int sensor) {
  if (sensor == 1) {
    return analogRead(PIN_MOISTURE_SENSOR_1);
  } else if (sensor == 2) {
    return analogRead(PIN_MOISTURE_SENSOR_2);
  }
}

int getSunData() {
  return analogRead(PIN_SUN_SENSOR);
}

int getRainData() {
  return analogRead(PIN_RAIN_SENSOR);
}

float getVoltage(int vol) {
  if (vol == 1) {
    float bat = analogRead(PIN_BATTERY_VOLTAGE);
    return (bat / 1023.0) * 16.6;
  } else if (vol == 2) {
    float pv = analogRead(PIN_PV_VOLTAGE);
    return (pv / 1023.0) * 30.0;
  }
}

bool getWaterLevel(int sensor) {
  if (sensor == 1) {
    return digitalRead(PIN_WATER_LEVEL_SENSOR_1);
  } else if (sensor == 2) {
    return digitalRead(PIN_WATER_LEVEL_SENSOR_2);
  }
}

void switchPump(int pump, int val) {
  if (pump == 1) {
    digitalWrite(PIN_PUMP_1, !val);
  } else if (pump == 2) {
    digitalWrite(PIN_PUMP_2, !val);
  }
}

void errorAndHold() {
  while (1) {
    digitalWrite(PIN_DATA_SAVING_LED, HIGH);
    delay(1000);
    digitalWrite(PIN_DATA_SAVING_LED, LOW);
    delay(1000);
  }
}

void pumpSafetyLevelCheck(){
  if(getWaterLevel(1) == 1){
      digitalWrite(PIN_PUMP_1, HIGH);
    }
  if(getWaterLevel(2) == 1){
      digitalWrite(PIN_PUMP_2, HIGH);
    }
}

void loop(void) {

  int ground_moisture_1_total = 0;
  int ground_moisture_2_total = 0;
  //taking the average of a few readings to get cleaner data
  for (int a = 0; a < NUMBER_OF_READINGS_TO_AVERAGE; a++) {
    ground_moisture_1_total += getMoistureSensorData(1);
    ground_moisture_2_total += getMoistureSensorData(2);
    delay(60000 / NUMBER_OF_READINGS_TO_AVERAGE);
  }

  int ground_moisture_1 = ground_moisture_1_total / NUMBER_OF_READINGS_TO_AVERAGE;
  int ground_moisture_2 = ground_moisture_2_total / NUMBER_OF_READINGS_TO_AVERAGE;

  if ((ground_moisture_1 < LOW_MOISTURE_LEVEL_1) || ((millis() - last_watering_1) > MAX_NO_WATERING_TIME)) {
    if((millis() - last_watering_1) > MIN_NO_WATERING_TIME){
      Serial.print("Max watering time p1: ");
      Serial.println(((millis() - last_watering_1) > MAX_NO_WATERING_TIME));
      Serial.print("Min watering time p1: ");
      Serial.println((millis() - last_watering_1) > MIN_NO_WATERING_TIME);
    if (ground_moisture_1 > 200) {
      last_watering_1 = millis();
      //checking if there is water inside the tank
      //there is a pullup, pin is shorted to GND if there is water
      if (getWaterLevel(1) == 0) {
        Serial.println("Watering with pump 1...");
        switchPump(1, 1);
        long int end_time = (WATERING_TIME_SECONDS_1 * 1000L) + millis();
        while (millis() < end_time) {
          saveWateringDataToSD(watering_counter_1, 1, millis(), getTemperature(temp_ground1), getTemperature(temp_ground2), getTemperature(temp_air), ground_moisture_1, ground_moisture_2, getWaterLevel(1), getWaterLevel(2), getVoltage(1), getVoltage(2), getSunData(), getRainData());
          pumpSafetyLevelCheck();
          delay((WATERING_TIME_SECONDS_1 * 1000.0) / DATA_READINGS_WHILE_WATERING);
        }
        switchPump(1, 0);
        watering_counter_1++;
        Serial.println("Watering Finished");
      }
    } else {
      Serial.println("Error, Moisture 1 level < 200");
    }
    }
  }

  if ((ground_moisture_2 < LOW_MOISTURE_LEVEL_2) || ((millis() - last_watering_2) > MAX_NO_WATERING_TIME)) {
    if((millis() - last_watering_2) > MIN_NO_WATERING_TIME){
      Serial.print("Max watering time p2: ");
      Serial.println(((millis() - last_watering_2) > MAX_NO_WATERING_TIME));
      Serial.print("Min watering time p2: ");
      Serial.println((millis() - last_watering_2) > MIN_NO_WATERING_TIME);
    if (ground_moisture_2 > 200) {
      last_watering_2 = millis();
      //checking if there is water inside the tank
      //there is a pullup, pin is shorted to GND if there is water
      if (getWaterLevel(2) == 0) {
        Serial.println("Watering with pump 2...");
        switchPump(2, 1);
        long int end_time = (WATERING_TIME_SECONDS_1 * 1000L) + millis();
        while (millis() < end_time) {
          saveWateringDataToSD(watering_counter_2, 2, millis(), getTemperature(temp_ground1), getTemperature(temp_ground2), getTemperature(temp_air), ground_moisture_1, ground_moisture_2, getWaterLevel(1), getWaterLevel(2), getVoltage(1), getVoltage(2), getSunData(), getRainData());
          pumpSafetyLevelCheck();
          delay((WATERING_TIME_SECONDS_2 * 1000.0) / DATA_READINGS_WHILE_WATERING);
        }
        switchPump(2, 0);
        watering_counter_2++;
        Serial.println("Watering Finished");
      }
    } else {
      Serial.println("Error, Moisture 2 level < 200");
    }
    }
  }
  Serial.println("Saving general data to SD");
  saveGeneralDataToSD(millis(), getTemperature(temp_ground1), getTemperature(temp_ground2), getTemperature(temp_air), ground_moisture_1, ground_moisture_2, getWaterLevel(1), getWaterLevel(2), getVoltage(1), getVoltage(2), getSunData(), getRainData());
}
