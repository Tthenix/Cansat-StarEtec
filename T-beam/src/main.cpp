#include <Arduino.h>

#include "../include/SensorServicio.hpp" // Incluimos archivos de cabeceras correspondientes
#include "../include/LoraService.hpp"    // Incluimos archivos de cabeceras correspondientes

SensorServicio servicioParaSensores; // Instanciamos el objeto servicioParaSensores de la clase SensorService
LoraService servicioParaLora;        // Instanciamos el objeto servicioParaLora de la clase LoraService

void setup()
{

  servicioParaSensores.begin(); // Método para inicializar los sensores
  servicioParaLora.begin();     // Método para inicializar la comunicación LoRa
}

void loop()
{
  int sizeValues = 20;
  float *ValoresDeSensores = new float[sizeValues]; // Creamos el array de float inicializados en cero
  float valorAnteriorTF = servicioParaSensores.getDistanciaTF();

  servicioParaSensores.leerSensores();                             // Método para realizar la lectura de los sensores
  servicioParaSensores.mostrarValores();                           // Método para realizar la impresión de valores leidos
  servicioParaSensores.formatearData(ValoresDeSensores);           // Formateamos los datos a un vector de float
  servicioParaLora.enviarDatosLoRa(ValoresDeSensores, sizeValues); // Se envían los datos del vector por LoRa

  delete[] ValoresDeSensores; // Liberar la memoria asignada con new
  delay(100);
}