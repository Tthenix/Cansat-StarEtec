#include <LoRa.h>
#include <SPI.h>
#include <Arduino.h>
#include "../include/LoraService.hpp"

LoraService::LoraService()
{
    // Inicializa tus variables y constantes aquí, si es necesario.
    counter = 0;
}

void LoraService::begin()
{
    // Inicializa la comunicación LoRa aquí, configurando frecuencia, potencia, etc.
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(BAND))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    Serial.println("LoRa Inicializacion OK!");
}

bool LoraService::sendSensorData(const float *data, size_t size)
{
    // Implementa aquí el envío de datos a través de LoRa.
    // Puedes usar la función LoRa.beginPacket() y LoRa.write().
    // No olvides finalizar el envío con LoRa.endPacket().

    LoRa.beginPacket();
    LoRa.print("Contador ");
    LoRa.print(counter);
    LoRa.endPacket();

    counter++;

    delay(10000);

    // Retorno temporal para cumplir con la firma de la función.
    return true;
}

// Función para enviar los datos por LoRa
void LoraService::enviarDatosLoRa(float *data, int dataSize)
{
    LoRa.beginPacket();
    LoRa.print("Vector de datos ");

    // Envía los datos por LoRa
    for (int i = 0; i < dataSize; i++)
    {
        LoRa.write((byte *)&data[i], sizeof(float));
    }
    LoRa.endPacket();
}

bool LoraService::receiveConfirmation()
{
    // Implementa aquí la recepción de confirmación a través de LoRa.
    // Puedes usar la función LoRa.parsePacket() para verificar si hay datos disponibles.
    // Lee los datos recibidos y comprueba si son una confirmación válida.

    if (LoRa.parsePacket())
    {
        // Leer los datos recibidos
        while (LoRa.available())
        {
            // Lee los datos y realiza la lógica de confirmación aquí
            // Por ejemplo, verifica si los datos son una confirmación válida
            // y devuelve true o false según corresponda.
        }
    }

    return false; // Si no se recibió confirmación válida
}

bool LoraService::reconnect()
{
    // Implementa aquí la lógica para reconectar si la comunicación LoRa se pierde.
    // Puedes usar esta función para intentar restablecer la conexión si algo sale mal.

    return false;
}
