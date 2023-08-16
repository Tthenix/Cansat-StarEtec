#include "../include/SensorServicio.hpp"

SensorServicio::SensorServicio()
{
}

void SensorServicio::begin()
{
    if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }

}

void SensorServicio::leerSensores()
{
bmpTemp = bmp.readTemperature();
bmpPres = bmp.readPressure();
bmpPressea = bmp.readSealevelPressure();
bmpAlt = bmp.readAltitude();
bmpRealAlt = bmp.readAltitude(102000);
}

void SensorServicio::mostrarValores()
{
 Serial.println("###########################");
  Serial.println("|IMPRESIÓN DE DATOS LEIDOS|");
  Serial.println("###########################");

    Serial.print("Temperature = ");
  Serial.print(bmpTemp);
  Serial.println(" *C");

    Serial.print("Pressure = ");
  Serial.print(bmpPres);
  Serial.println(" Pa");

      Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmpPressea);
  Serial.println(" Pa");

    Serial.print("Altitude = ");
  Serial.print(bmpAlt);
  Serial.println(" meters");

    Serial.print("Real altitude = ");
  Serial.print(bmpRealAlt);
  Serial.println(" meters");



  Serial.println("-----------------------------------------------");
}