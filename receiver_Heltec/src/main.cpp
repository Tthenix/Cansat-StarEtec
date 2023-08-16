#include <Arduino.h>

#include "../include/SensorServicio.hpp" // Incluimos archivos de cabeceras correspondientes
SensorServicio servicioParaSensores;     // Instanciamos el objeto servicioParaSensores de la clase SensorService

void setup()
{
  Serial.begin(115200);
  servicioParaSensores.begin();
  Serial.println("sensorService");
}

void loop()
{
  servicioParaSensores.leerSensores();
  Serial.println("Valores leídos:");
  servicioParaSensores.mostrarValores();

  delay(1000); // Aumentamos el retardo para observar los valores con calma
}