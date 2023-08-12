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
  float *ValoresDeSensores = new float[sizeValues];                                   // Creamos el array de float inicializados en cero

  servicioParaSensores.leerSensores();   // Método para realizar la lectura de los sensores
  servicioParaSensores.mostrarValores(); // Método para realizar la impresión de valores leidos

  servicioParaSensores.formatearData(ValoresDeSensores);            // Formateamos los datos a un vector de float
  servicioParaLora.enviarDatosLoRa(ValoresDeSensores, sizeValues); // Se envían los datos del vector por LoRa

  delete[] ValoresDeSensores; // Liberar la memoria asignada con new
  delay(3000);
}


/*

#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet with ");
    Serial.print(packetSize);
    Serial.println(" bytes");

    while (LoRa.available()) {
      float receivedValue;
      LoRa.readBytes((byte *)&receivedValue, sizeof(float));
      Serial.print("Received value: ");
      Serial.println(receivedValue, 4); // Imprime con 4 decimales
    }

    Serial.println("Packet received");
  }
}


*/