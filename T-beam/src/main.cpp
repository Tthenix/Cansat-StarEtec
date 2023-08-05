#include <Arduino.h>

#include "../include/SensorServicio.hpp"
#include "../include/LoraService.hpp"

SensorServicio sensorServicio;
LoraService loraService;

void setup()
{
  sensorServicio.begin();
 
}

void loop()
{
  loraService.begin();
  sensorServicio.leerSensores();
  sensorServicio.mostrarValores();

  delay(1000);
}
