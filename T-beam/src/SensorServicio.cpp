#include "../include/SensorServicio.hpp"

SensorServicio::SensorServicio() : 
  dht(DHTPIN, DHTTYPE),
  TFminiSerial(TFMINI_RX_PIN, TFMINI_TX_PIN),
  NEO6mSerial(GPS_SERIAL_NUM)
{
  // Defino el pin donde estará el sensor MQ7
  mq7Pin = 14;
}

void SensorServicio::begin() {
  M5.begin();
  Serial.begin(SerialUsb_BAUDRATE);
  dht.begin();
  if(!bmp.begin()){
      while(true);
  }

  // SoftwareSerial  TFminiSerial(TFMINI_RX_PIN, TFMINI_TX_PIN); // UART1 - Pins GPIO16 (RX) y GPIO17 (TX)
  // HardwareSerial NEO6mSerial(GPS_SERIAL_NUM);    // UART2 - Pins GPIO17 (RX) y GPIO16 (TX)

  NEO6mSerial.begin(GPS_BAUDRATE);   // Inicia la comunicación con el GPS

  TFmini.setBaudRate(TFMINI_BAUDRATE);
  TFmini.begin(&TFminiSerial);
}

void SensorServicio::leerSensores() {
  //*** MQ7 values ***/    
  //    Para revisar, calibración y medidas del COppm
  // CO_ppm = (AnalogValue - Offset) * (MaxPPM / (MaxValue - Offset))

  mq7VoltageValue = analogRead(mq7Pin);
  // mq7data = adc_MQ * (5.0 / 1023.0);
  mq7COppm = (mq7VoltageValue - mq7Offset) * (mq7MaxPPM / (mq7MaxValue - mq7Offset));

  //*** AXP192 values ***/
  vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  vaps = M5.Axp.GetVapsData() * 1.4 / 1000;
  icharge = M5.Axp.GetIchargeData() / 2.0;
  idischarge = M5.Axp.GetIdischargeData() / 2.0;
  tempAXP192 = M5.Axp.GetTempInAXP192();

  //*** DHT values ***/
  dhtHum = dht.readHumidity();
  dhtTemp = dht.readTemperature();
  dhtTempF = dht.readTemperature(true);

  //*** BMP values ***/
  bmpTemp = bmp.readTemperature();
  bmpPres = bmp.readPressure();
  bmpAlti = bmp.readAltitude();

  //*** TFmini values ***/
  getTFminiData();

  //*** NEO6M values ***/
  readGPS();
}

void SensorServicio::mostrarValores() {

  Serial.println("###############################################");
  Serial.println("|IMPRESIÓN DE DATOS LEIDOS|");

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
  Serial.println(" V");
  Serial.print("\tVaps: ");
  Serial.print(vaps, 3);
  Serial.println(" V");
  Serial.print("\tIcharge: ");
  Serial.print(icharge, 3);
  Serial.println(" mA");
  Serial.print("\tIdischarge: ");
  Serial.print(idischarge, 3);
  Serial.println(" mA");
  Serial.print("\tTemperatura: ");
  Serial.print(tempAXP192);
  Serial.println(" °C");


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
  Serial.println(" degr");
  

  Serial.println("-----------------------------------------------");
  Serial.println("Sensor TFmini");  
  
  Serial.print("\tDistancia : ");
  Serial.print(TFdistance);
  Serial.print("cm\t");
  Serial.print("Fortaleza de Señal: ");
  Serial.println(TFstrength);
  Serial.print("\tTemperatura TFmini: ");
  Serial.println(TFtemp);
  

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
  Serial.println(" m/s");
  Serial.print("\tFecha: ");
  Serial.print(gpsDate);
  Serial.print("\tHora: ");
  Serial.println(gpsTime);
  
}


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


void SensorServicio::readGPS() {
  // Leer datos del GPS y almacénalos en la librería TinyGPS++
  while (NEO6mSerial.available() > 0) {
    if (gps.encode(NEO6mSerial.read())) {
      // Si los datos del GPS están disponibles y se decodifican correctamente,
      // actualiza los parámetros por referencia con la información relevante.
      if (gps.location.isValid()) {
        gpsLatitude = gps.location.lat();
        gpsLongitude = gps.location.lng();
      }

      if (gps.altitude.isValid()) {
        gpsLatitude = gps.altitude.meters();
      }

      if (gps.speed.isValid()) {
        gpsSpeed = gps.speed.mps();
      }

      if (gps.date.isValid() && gps.time.isValid()) {
        gpsDate = gps.date.value();
        gpsTime = gps.time.value();
      }
    }
  }
}

void SensorServicio::formatearData(float* data) {
  // Leemos nuevamente los sensores antes de enviarlos
  leerSensores();
  // Cargamos cada dato donde corresponde en el array
  data[0] = mq7Pin;
  data[1] = mq7VoltageValue;
  data[2] = mq7COppm;
  data[3] = vbat;
  data[4] = vaps;
  data[5] = icharge;
  data[6] = idischarge;
  data[7] = tempAXP192;
  data[8] = dhtHum;
  data[9] = dhtTemp;
  data[10] = dhtTempF;
  data[11] = bmpPres;
  data[12] = bmpTemp;
  data[13] = bmpAlti;
  data[14] = TFdistance;
  data[15] = TFstrength;
  data[16] = TFtemp;
  data[17] = gpsLatitude;
  data[18] = gpsLongitude;
  data[19] = gpsAltitude;
}