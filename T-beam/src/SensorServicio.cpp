#include "../include/SensorServicio.hpp"

SensorServicio::SensorServicio() : dht(DHTPIN, DHTTYPE)
          ,TFminiSerial(TFMINI_TX_PIN, TFMINI_RX_PIN)
          ,gpsSerial(GPS_TX_PIN, GPS_RX_PIN)
{

  // Se definen parametros de seteo para MQ7
  mq7Pin = 14;  // Defino el pin donde estará el sensor MQ7
  mq7Offset = 0.0;
  mq7MaxPPM = 5.0;  
  mq7MaxValue = 1023.0;

  // Ajuste de la zona horaria local (UTC-3 para Mendoza, Argentina)
  timeZoneOffset = -3;
  
  // Se definen parametros de seteo para AXP192
  deltaT = 1.0;
  
  // Habilitar el pull-up interno para los pines SDA y SCL
  // i2cSetPin(BMP_SDA, true);
  // i2cSetPin(BMP_SCL, true);
}

void SensorServicio::begin()
{
  gpsSerial.begin(9600); // Inicia la comunicación con el GPS
  Serial.begin(SerialUsb_BAUDRATE);
  dht.begin();

  M5.begin();
   
  // BMP init
  unsigned status;
  status = bmp.begin(BMP280_ADDRESS_new);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("        ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
    /* Default settings from the datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X8,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  

  TFminiSerial.begin(TFMINI_BAUDRATE);
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
  // (deprecated) //
  // vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  // vaps = M5.Axp.GetVapsData() * 1.4 / 1000;
  // icharge = M5.Axp.GetIchargeData() / 2.0;
  // idischarge = M5.Axp.GetIdischargeData() / 2.0;
  // tempAXP192 = M5.Axp.GetTempInAXP192();
  // (deprecated) // EOF

  vbat = M5.Axp.GetBatVoltage();
  vaps = M5.Axp.GetAPSVoltage();
  icharge = M5.Axp.GetBatCurrent();
  idischarge = M5.Axp.GetBatChargeCurrent();
  tempAXP192 = M5.Axp.GetTempInAXP192();

  // Leer el tiempo transcurrido desde el último ciclo de bucle en segundos
  // float deltaT = 1.0; // Ajusta este valor según la frecuencia de lectura
  float current;
  if((idischarge <= 0)&&(icharge > 0)){
    current = icharge;
  } else if((idischarge > 0)&&(icharge <= 0)){
    current = idischarge;
  } else { 
    current = 0;
  }
  mAh = (current) * (deltaT / 3600.0);


  //*** DHT values ***/  
  dhtHum = dht.readHumidity();
  dhtTemp = dht.readTemperature();
  dhtTempF = dht.readTemperature(true);

  // must call this to wake sensor up and get new measurement data
  // it blocks until measurement is complete
  if (bmp.takeForcedMeasurement()) {
    //*** BMP values ***/
    bmpTemp = bmp.readTemperature();
    bmpPres = bmp.readPressure() / 100;   // dividir por 100 para tener hPa
    bmpAlti = bmp.readAltitude(1013.25);
  }

  // /*** TFmini values ***/
  this->getTFminiData();
  // TFdistance = TFmini.getDistance();
  // TFtemp = TFmini.getSensorTemperature();
  // TFstrength = TFmini.getSignalStrength();

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

  Serial.print("\tDistancia : ");
  Serial.print(TFdistance);
  Serial.print(" cm");
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
  if(gpsSerial.available()){
    this->readGPS();
  } else {
    Serial.println("%%%%  GPS Serial not available!  %%%");
  }
  
}

// Método para formatear la data del TFmini s [LIDAR]
void SensorServicio::getTFminiData() {
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static int rx[9];
  if(TFminiSerial.available())
  {
    // Serial.println( "tfmini serial available" );
    rx[i] = TFminiSerial.read();
    if(rx[0] != 0x59) {
      i = 0;
    } else if(i == 1 && rx[1] != 0x59) {
      i = 0;
    } else if(i == 8) {
      for(j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if(rx[8] == (checksum % 256)) {
        TFdistance = rx[2] + rx[3] * 256;
        TFstrength = rx[4] + rx[5] * 256;
      }
      i = 0;
    } else
    {
      i++;
    } 
  }
}

// Metodo para leer todos los parámetros del GPS Neo 6M
void SensorServicio::readGPS()
{  
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        gpsLatitude = gps.location.lat();
        gpsLongitude = gps.location.lng();
        
        // Obtener la altitud y velocidad
        gpsAltitude = gps.altitude.meters();
        gpsSpeed = gps.speed.kmph();

        // Obtener la fecha y hora
        if (gps.date.isValid()) {          
          if (gps.time.isValid()) {
            gpsHour = gps.time.hour();
            gpsDay = gps.date.day();
            // Ajustar la hora a la zona horaria local (UTC-3 para Mendoza, Argentina)
            gpsHour += timeZoneOffset;
            if (gpsHour < 0) {
              gpsHour += 24;
              gpsDay--;
            } else if (gpsHour >= 24) {
              gpsHour -= 24;
              gpsDay++;
            }
            gpsHour;
            gpsMinutes = gps.time.minute();
            gpsSeconds = gps.time.minute();
          } else {
            Serial.println("Hora no válida");
          }

          gpsYear = gps.date.year(); 
          gpsMonth = gps.date.month(); 
        } else {
          Serial.println("Fecha no válida");
        }
        Serial.println();

      } else {
        Serial.println("Sin señal GPS");
      }
    }
  }
}


// void SensorServicio::formatearData(float *data)
// {
//   // Leemos nuevamente los sensores antes de enviarlos
//   // leerSensores();
//   // Cargamos cada dato donde corresponde en el array
//   data[0] = mq7Pin;
//   data[1] = mq7VoltageValue;
//   data[2] = mq7COppm;
//   data[3] = mAh;
//   data[4] = vbat;
//   data[5] = vaps;
//   data[6] = icharge;
//   data[7] = idischarge;
//   data[8] = tempAXP192;
//   data[9] = dhtHum;
//   data[10] = dhtTemp;
//   data[11] = dhtTempF;
//   // data[12] = bmpPres;
//   // data[13] = bmpTemp;
//   // data[14] = bmpAlti;
//   // data[12] = TFdistance;
//   // data[13] = TFstrength;
//   // data[14] = TFtemp;
//   // data[18] = gpsLatitude;
//   // data[19] = gpsLongitude;
//   // data[20] = gpsAltitude;
// }