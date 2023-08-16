#ifndef SENSOR_SERVICIO_H
#define SENSOR_SERVICIO_H

// #include <HardwareSerial.h>
#include "SoftwareSerial.h"
#include "Wire.h"

#include <M5StickC.h>
#include "AXP192.h" //a
#include "DHT.h"
#include <Adafruit_BMP280.h>

// #include "Adafruit_Sensor.h"
// #include <TFMiniPlus.h>
#include "TFMini.h"
#include "TinyGPSPlus.h"

// Parámetros a tener en cuenta con el DHT11
#define DHTPIN 13
#define DHTTYPE DHT11

// Parámetros a tener en cuenta con el USB
#define SerialUsb_BAUDRATE 115200
#define SerialUsb_NUM 0
#define SerialUsb_RX_PIN 3
#define SerialUsb_TX_PIN 1

// Parámetros a tener en cuenta con el Neo6m
#define GPS_BAUDRATE 9600
// #define GPS_SERIAL_NUM 2
#define GPS_TX_PIN 34
#define GPS_RX_PIN 12

// Parámetros a tener en cuenta con el TFmini
#define TFMINI_BAUDRATE 115200
// En uso probablemente con el AXP192.
// #define TFMINI_SERIAL_NUM 2
// Se tiene quer invertir en software y hardware
#define TFMINI_RX_PIN 33
#define TFMINI_TX_PIN 25

// Parámetros a tener en cuenta con el BMP280
#define BMP280_ADDRESS_new (0x76)
#define BMP_SDA 21 // Pin SDA conectado al pin 21 de la placa LilyGO
#define BMP_SCL 22 // Pin SCL conectado al pin 22 de la placa LilyGO

class SensorServicio
{
public:
  SensorServicio();

  void begin();
  void leerSensores();
  void mostrarValores();
  void formatearData(float *data);

  //*** DHT11 Getters ***/
  float getHumedadDHT() const { return dhtHum; }
  float getTemperaturaDHT() const { return dhtTemp; }
  float getTemperaturaFhDHT() const { return dhtTempF; }

  //*** TFmini Getters ***/
  float getVoltajeMQ7() const { return analogRead(14); }

  //*** BMP280 Getters ***/
  float getTemperaturaBMP() const { return bmpTemp; }
  float getAltitudBMP() const { return bmpAlti; }
  float getPresionBMP() const { return bmpPres; }

  //*** TFmini Getters ***/
  void getTFminiData();
  float getDistanciaTF() const { return TFdistance; }
  float getFortalezaTF() const { return TFstrength; }

  //*** AXP192 Getters ***/
  float getVbat() const { return vbat; }
  float getVaps() const { return vaps; }
  float getIcharge() const { return icharge; }
  float getIdischarge() const { return idischarge; }
  float getTempAXP192() const { return tempAXP192; }

  //*** NEO6M Getters ***/
  void readGPS();
  float getAltitudGPS() const { return gpsAltitude; }
  float getLatitudGPS() const { return gpsLatitude; }
  float getLongitudGPS() const { return gpsLongitude; }
  float getVelocidadGPS() const { return gpsSpeed; }
  uint8_t gpsYearGPS() const { return gpsYear; }
  uint8_t gpsMonthGPS() const { return gpsMonth; }
  uint8_t gpsDayGPS() const { return gpsDay; }
  uint8_t gpsHourGPS() const { return gpsHour; }
  uint8_t gpsMinutesGPS() const { return gpsMinutes; }
  uint8_t gpsSecondsGPS() const { return gpsSeconds; }

private:
  //*** DHT values ***/
  DHT dht;
  float dhtHum;
  float dhtTemp;
  float dhtTempF;

  //*** BMP values ***/
  Adafruit_BMP280 bmp;
  float bmpTemp;
  float bmpPres;
  float bmpAlti;

  //*** MQ7 values ***/
  int mq7Pin;
  float mq7VoltageValue;
  float mq7COppm;
  float mq7Offset;
  float mq7MaxPPM;
  float mq7MaxValue;

  //*** AXP192 values ***/
  AXP192 axp;
  float vbat;
  float vaps;
  float icharge;
  float idischarge;
  float tempAXP192;
  float mAh;
  float deltaT;

  //*** TFmini values ***/
  SoftwareSerial TFminiSerial;

  TFMini TFmini;
  float TFdistance;
  float TFstrength;

  float TFdistanceAverage = 0.0;
  static const int numReadings = 3; // Número de lecturas para evaluar la precisión
  int readings[numReadings];        // Últimas lecturas de distancia
  int readIndex = 0;                // Índice de lectura actual
  int total = 0;                    // Suma de las lecturas
  unsigned long previousMillis = 0; // Hora de la última medición
  const unsigned long interval = 0; // Intervalo entre mediciones (milisegundos)

  // Resto de la implementación de la clase...

  //*** NEO6M values ***/
  SoftwareSerial gpsSerial;
  TinyGPSPlus gps;
  int timeZoneOffset;
  float gpsLatitude;
  float gpsLongitude;
  float gpsAltitude;
  float gpsSpeed;
  uint8_t gpsYear;
  uint8_t gpsMonth;
  uint8_t gpsDay;
  uint8_t gpsHour;
  uint8_t gpsMinutes;
  uint8_t gpsSeconds;
};

#endif // SENSOR_SERVICIO_H