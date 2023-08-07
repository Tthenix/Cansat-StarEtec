#ifndef SENSOR_SERVICIO_H
#define SENSOR_SERVICIO_H

// #include <HardwareSerial.h>
#include "SoftwareSerial.h"
#include "Wire.h"

#include <M5StickC.h>
#include "DHT.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_Sensor.h"
#include "TFMiniPlus.h"
#include "TinyGPSPlus.h"

// Parámetros a tener en cuenta con el DHT11
#define DHTPIN 25
#define DHTTYPE DHT11

// Parámetros a tener en cuenta con el USB
#define SerialUsb_BAUDRATE 115200
#define SerialUsb_NUM 0
#define SerialUsb_RX_PIN 3
#define SerialUsb_TX_PIN 1

// Parámetros a tener en cuenta con el Neo6m
#define GPS_BAUDRATE 9600
// #define GPS_SERIAL_NUM 2
#define GPS_RX_PIN 34
#define GPS_TX_PIN 12

// Parámetros a tener en cuenta con el TFmini
#define TFMINI_BAUDRATE 115200
// En uso probablemente con el AXP192.
// #define TFMINI_SERIAL_NUM 2 
// Se tiene quer invertir en software y hardware
#define TFMINI_RX_PIN 35                                                                                                                                                                                        
#define TFMINI_TX_PIN 32

// Parámetros a tener en cuenta con el BMP280
#define BMP280_ADDRESS 0x76
#define BMP_SDA 21   // Pin SDA conectado al pin 21 de la placa LilyGO
#define BMP_SCL 22   // Pin SCL conectado al pin 22 de la placa LilyGO

class SensorServicio
{
public:
    SensorServicio();

    void begin();
    void leerSensores();
    void mostrarValores();
    void formatearData(float* data);

    //*** DHT11 Getters ***/
    float getHumedadDHT() const { return dhtHum; }
    float getTemperaturaDHT() const { return dhtTemp; }
    float getTemperaturaFhDHT() const { return dhtTempF; }

    //*** TFmini Getters ***/
    float getVoltajeMQ7() const { return analogRead(14);}

    //*** BMP280 Getters ***/
    float getTemperaturaBMP() const { return bmpTemp;}
    float getAltitudBMP() const { return bmpAlti;}
    float getPresionBMP() const { return bmpPres;}

    //*** TFmini Getters ***/

    void getTFminiData();
    float getDistanciaTF() const { return TFdistance;}
    float getTemperaturaTF() const { return TFtemp;}
    float getFortalezaTF() const { return TFstrength;}

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
    unsigned long getFechaGPS() const { return gpsDate; }
    unsigned long getHoraGPS() const { return gpsTime; }

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
    int mq7VoltageValue; 
    float mq7COppm;
    float mq7Offset;
    float mq7MaxPPM;
    float mq7MaxValue;

    //*** AXP192 values ***/
    float vbat;
    float vaps;
    float icharge;
    float idischarge;
    float tempAXP192;
    float mAh;
    float deltaT;

    //*** TFmini values ***/
    // SoftwareSerial  TFminiSerial;
    TFMiniPlus TFmini;
    float TFdistance;
    float TFtemp;
    float TFstrength;
    
    //*** NEO6M values ***/
    // SoftwareSerial NEO6mSerial;
    TinyGPSPlus gps;
    float gpsLatitude;
    float gpsLongitude;
    float gpsAltitude;
    float gpsSpeed;
    unsigned long gpsDate;
    unsigned long gpsTime;
};

#endif // SENSOR_SERVICIO_H
