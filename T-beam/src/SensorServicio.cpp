#include "../include/SensorServicio.hpp"
#include <Wire.h>
#include "AXP192.h"

SensorServicio::SensorServicio() : dht(DHTPIN, DHTTYPE), TFminiSerial(TFMINI_TX_PIN, TFMINI_RX_PIN), gpsSerial(GPS_TX_PIN, GPS_RX_PIN)
{

  // Se definen parametros de seteo para MQ7
  mq7Pin = 14; // Defino el pin donde estará el sensor MQ7
  mq7Offset = 0.0;
  mq7MaxPPM = 5.0;
  mq7MaxValue = 1023.0;

  // Ajuste de la zona horaria local (UTC-3 para Mendoza, Argentina)
  timeZoneOffset = -3;

  // Se definen parametros de seteo para AXP192
  deltaT = 1.0;
}

void SensorServicio::begin()
{
  gpsSerial.begin(9600); // Inicia la comunicación con el GPS
  Serial.begin(SerialUsb_BAUDRATE);
  dht.begin();

  // M5.begin(false, true, false);
  axp.begin();

  // BMP init
  unsigned status;
  status = bmp.begin(BMP280_ADDRESS_new);
  if (!status)
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(bmp.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("        ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1)
      delay(10);
  }

  /* Default settings from the datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,  /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X8,  /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16, /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16);  /* Standby time. */

  TFminiSerial.begin(TFMINI_BAUDRATE);
  delay(4000);
  TFmini.begin(&TFminiSerial);
}

void SensorServicio::leerSensores()
{
  //*** MQ7 values ***/
  //    Para revisar, calibración y medidas del COppm
  // CO_ppm = (AnalogValue - Offset) * (MaxPPM / (MaxValue - Offset))

  mq7COppm = (analogRead(mq7Pin) - mq7Offset) * (mq7MaxPPM / (mq7MaxValue - mq7Offset));
  mq7VoltageValue = analogRead(mq7Pin) * (3.3 / 1023.0);

  //*** AXP192 values ***/
  vbat = axp.GetBatVoltage();
  vaps = axp.GetAPSVoltage();
  icharge = axp.GetBatCurrent();
  idischarge = axp.GetBatChargeCurrent();
  tempAXP192 = axp.GetTempInAXP192();

  // Leer el tiempo transcurrido desde el último ciclo de bucle en segundos
  // float deltaT = 1.0; // Ajusta este valor según la frecuencia de lectura
  float current;
  if ((idischarge <= 0) && (icharge > 0))
  {
    current = icharge;
  }
  else if ((idischarge > 0) && (icharge <= 0))
  {
    current = idischarge;
  }
  else
  {
    current = 0;
  }
  mAh = (current) * (deltaT / 3600.0);

  //*** DHT values ***/
  dhtHum = dht.readHumidity();
  dhtTemp = dht.readTemperature();
  dhtTempF = dht.readTemperature(true);

  // must call this to wake sensor up and get new measurement data
  // it blocks until measurement is complete
  if (bmp.takeForcedMeasurement())
  {
    //*** BMP values ***/
    bmpTemp = bmp.readTemperature();
    bmpPres = bmp.readPressure() / 100; // dividir por 100 para tener hPa
    bmpAlti = bmp.readAltitude(1013.25);
  }

  // /*** TFmini values ***/
  this->getTFminiData();

  /*** NEO6M values ***/
  this->readGPS();
}

void SensorServicio::mostrarValores()
{

  Serial.println("###########################");
  Serial.println("|IMPRESIÓN DE DATOS LEIDOS|");
  Serial.println("###########################");

  Serial.println("Sensor MQ7");

  Serial.print("\tPin: ");
  Serial.println(mq7Pin);
  Serial.print("\tVoltaje: ");
  Serial.print(mq7VoltageValue);
  Serial.println(" V");
  Serial.print("\tPartes por millon de CO: ");
  Serial.print(mq7COppm);
  Serial.println(" COppm");

  Serial.println("-----------------------------------------------");
  Serial.println("Sensor AXP192");

  Serial.print("\tVbat: ");
  Serial.print(vbat, 3);
  Serial.print(" V");
  Serial.print("\t|  Vaps: ");
  Serial.print(vaps, 3);
  Serial.println(" V");
  Serial.print("\tIcharge: ");
  Serial.print(icharge, 3);
  Serial.print(" mA");
  Serial.print("\t|  Idischarge: ");
  Serial.print(idischarge, 3);
  Serial.println(" mA");
  Serial.print("\tTemperatura: ");
  Serial.print(tempAXP192);
  Serial.print(" °C");
  Serial.print("\t|  Consumo: ");
  Serial.print(mAh, 3);
  Serial.println(" mA/h");

  Serial.println("-----------------------------------------------");
  Serial.println("Sensor DHT11");

  Serial.print("\tHumedad DHT: ");
  Serial.print(dhtHum);
  Serial.println(" %t");
  Serial.print("\tTemperatura DHT: ");
  Serial.print(dhtTemp);
  Serial.println(" *C ");
  Serial.print("\tTemperatura (°F) DHT: ");
  Serial.print(dhtTempF);
  Serial.println(" *F");

  Serial.println("-----------------------------------------------");
  Serial.println("Sensor BMP280");

  Serial.print("\tPresión BMP: ");
  Serial.print(bmpPres);
  Serial.println(" hPa");
  Serial.print("\tTemperatura BMP: ");
  Serial.print(bmpTemp);
  Serial.println(" *C ");
  Serial.print("\tAltitud BMP: ");
  Serial.print(bmpAlti);
  Serial.println(" m");

  Serial.println("-----------------------------------------------");
  Serial.println("Sensor TFmini");

  Serial.print("\tDistancia promedio: ");
  Serial.print(TFdistanceAverage);
  Serial.println(" cm");
  Serial.print("\tFortaleza de Señal: ");
  Serial.println(TFstrength);

  Serial.println("-----------------------------------------------");
  Serial.println("Sensor Neo6-M");

  Serial.print("\tLatitud: ");
  Serial.println(gpsLatitude, 6);
  Serial.print("\tLongitud: ");
  Serial.println(gpsLongitude, 6);
  Serial.print("\tAltitud GPS: ");
  Serial.print(gpsAltitude);
  Serial.println(" metros");
  Serial.print("\tVelocidad: ");
  Serial.print(gpsSpeed);
  Serial.println(" km/h");
  Serial.print("\tFecha: ");
  Serial.print(gpsDay);
  Serial.print("/");
  Serial.print(gpsMonth);
  Serial.print("/");
  Serial.print(gpsYear);
  Serial.print("\t|  Hora: ");
  Serial.print(gpsHour);
  Serial.print(":");
  Serial.print(gpsMinutes);
  Serial.print(":");
  Serial.println(gpsSeconds);
  if (gpsSerial.available())
  {
    this->readGPS();
  }
  else
  {
    Serial.println("%%%%  GPS Serial not available!  %%%");
  }
}

// Método para formatear la data del TFmini s [LIDAR]
void SensorServicio::getTFminiData()
{
  // Obtener el tiempo actual
  unsigned long currentMillis = millis();

  // Comprobar si ha transcurrido el intervalo desde la última medición
  if (currentMillis - previousMillis >= interval)
  {
    // Guardar el tiempo actual para la próxima medición
    previousMillis = currentMillis;

    // Tomar una medición de distancia del TF Mini
    uint16_t dist = TFmini.getDistance();
    uint16_t strength = TFmini.getRecentSignalStrength();

    // Agregar la lectura actual al total y restar la lectura más antigua
    total = total - readings[readIndex];
    readings[readIndex] = dist;
    total = total + readings[readIndex];
    readIndex++;

    // Si el índice llega al final del array, volver a empezar
    if (readIndex >= numReadings)
    {
      readIndex = 0;
    }

    // Calcular el promedio de las lecturas
    int average = total / numReadings;

    // Verificar si las lecturas son coherentes
    bool consistent = true;
    for (int i = 0; i < numReadings; i++)
    {
      if (abs(readings[i] - average) > 5) // Ajusta este valor según la tolerancia deseada
      {
        consistent = false;
        break;
      }
    }

    // Mostrar el promedio solo si las lecturas son coherentes
    if (consistent)
    {
      TFdistanceAverage = average;
    }
  }
}

// Metodo para leer todos los parámetros del GPS Neo 6M
void SensorServicio::readGPS()
{
  while (gpsSerial.available() > 0)
  {
    if (gps.encode(gpsSerial.read()))
    {
      if (gps.location.isValid())
      {
        gpsLatitude = gps.location.lat();
        gpsLongitude = gps.location.lng();

        // Obtener la altitud y velocidad
        gpsAltitude = gps.altitude.meters();
        gpsSpeed = gps.speed.kmph();

        // Obtener la fecha y hora
        if (gps.date.isValid())
        {
          if (gps.time.isValid())
          {
            gpsHour = gps.time.hour();
            gpsDay = gps.date.day();
            // Ajustar la hora a la zona horaria local (UTC-3 para Mendoza, Argentina)
            gpsHour += timeZoneOffset;
            if (gpsHour < 0)
            {
              gpsHour += 24;
              gpsDay--;
            }
            else if (gpsHour >= 24)
            {
              gpsHour -= 24;
              gpsDay++;
            }
            gpsHour;
            gpsMinutes = gps.time.minute();
            gpsSeconds = gps.time.minute();
          }
          else
          {
            Serial.println("Hora no válida");
          }

          gpsYear = gps.date.year();
          gpsMonth = gps.date.month();
        }
        else
        {
          Serial.println("Fecha no válida");
        }
        Serial.println();
      }
      else
      {
        Serial.println("Sin señal GPS");
      }
    }
  }
}

void SensorServicio::formatearData(float *data)
{
  // Leemos nuevamente los sensores antes de enviarlos
  leerSensores();
  // Cargamos cada dato donde corresponde en el array
  data[0] = mq7Pin;
  data[1] = mq7VoltageValue;
  data[2] = mq7COppm;
  data[3] = mAh;
  data[4] = vbat;
  data[5] = vaps;
  data[6] = icharge;
  data[7] = idischarge;
  data[8] = tempAXP192;
  data[9] = dhtHum;
  data[10] = dhtTemp;
  data[11] = dhtTempF;
  data[12] = bmpPres;
  data[13] = bmpTemp;
  data[14] = bmpAlti;
  data[15] = TFdistanceAverage;
  data[16] = TFstrength;
  data[17] = gpsLatitude;
  data[18] = gpsLongitude;
  data[19] = gpsAltitude;
}