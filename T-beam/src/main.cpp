#include <Arduino.h>

#include "../include/SensorServicio.hpp"

SensorServicio sensorServicio;

void setup() {
  sensorServicio.begin();
}

void loop() {
  sensorServicio.leerSensores();
  sensorServicio.mostrarValores();
  delay(1000);
}
