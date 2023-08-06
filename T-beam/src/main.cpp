#include <Arduino.h>

#include "../include/SensorServicio.hpp"
#include "../include/LoraService.hpp"

SensorServicio sensorServicio;
LoraService loraService;

void setup()
{
  sensorServicio.begin();
  loraService.begin(); 
}

void loop()
{
  float ValoresDeSensores[20];
  
  sensorServicio.leerSensores();
  sensorServicio.mostrarValores();

  sensorServicio.formatearData(ValoresDeSensores);
  loraService.enviarDatosLoRa(ValoresDeSensores, 20);

  delay(1000);
}
