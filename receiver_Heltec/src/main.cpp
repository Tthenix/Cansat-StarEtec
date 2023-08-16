#include <Arduino.h>

#include "../include/SensorServicio.hpp" // Incluimos archivos de cabeceras correspondientes
SensorServicio servicioParaSensores; // Instanciamos el objeto servicioParaSensores de la clase SensorService

void setup()
{
    servicioParaSensores.begin(); 
}

void loop(){
  servicioParaSensores.leerSensores();   // Método para realizar la lectura de los sensores
servicioParaSensores.mostrarValores(); // Método para realizar la impresión de valores leidos

    delay(100);
}