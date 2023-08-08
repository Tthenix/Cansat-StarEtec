#include <Arduino.h>

#include "../include/SensorServicio.hpp"    // Incluimos archivos de cabeceras correspondientes
#include "../include/LoraService.hpp"    // Incluimos archivos de cabeceras correspondientes

SensorServicio servicioParaSensores;      // Instanciamos el objeto servicioParaSensores de la clase SensorService
// LoraService servicioParaLora;      // Instanciamos el objeto servicioParaLora de la clase LoraService

void setup()
{
  servicioParaSensores.begin();      // Método para inicializar los sensores
  // servicioParaLora.begin();      // Método para inicializar la comunicación LoRa
}

void loop()
{
  float* ValoresDeSensores = new float[20];     // Creamos el array de float inicializados en cero
  int sizeOfArray = sizeof(ValoresDeSensores) / sizeof(ValoresDeSensores[0]);     // Sacamos el tamaño del array al ser un array dinamico nos devolvería el tamaño del puntero

  servicioParaSensores.leerSensores();    // Método para realizar la lectura de los sensores
  servicioParaSensores.mostrarValores();    // Método para realizar la impresión de valores leidos


  // servicioParaSensores.formatearData(ValoresDeSensores);    // Formateamos los datos a un vector de float
  // servicioParaLora.enviarDatosLoRa(ValoresDeSensores, sizeOfArray);    // Se envían los datos del vector por LoRa

  delete[] ValoresDeSensores;   // Liberar la memoria asignada con new

  delay(1000);
}
