#include "../include/LoraService.hpp"

LoraService::LoraService()
{
    // Inicializa tus variables y constantes aquí, si es necesario.
    counter = 0;
}

void LoraService::begin()
{
    // Inicializa la comunicación LoRa aquí, configurando frecuencia, potencia, etc.
    // SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    // setup LoRa transceiver module
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

    // Retorno temporal para cumplir con la firma de la función.
    return true;
}

// Función para enviar los datos por LoRa
void LoraService::enviarDatosLoRa(float *data, int dataSize)
{
    Serial.print("Enviando paquete: ");
    Serial.println(counter);

    // Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print("Contador ");
    LoRa.print(counter);
    LoRa.endPacket();
    counter++;

    LoRa.beginPacket();
    LoRa.println("Vector de datos ");

    // float numeroFloat = 3.14159;
    // char cadena[20]; // Ajusta el tamaño según tus necesidades
    // sprintf(cadena, "%f", numeroFloat);
    // LoRa.printf("numero %s\n", cadena);
    // LoRa.print("el puto vector");

    char cadena[35]; // Ajusta el tamaño según tus necesidades
    // Envía los datos por LoRa
    for (int i = 0; i < dataSize; i++)
    {
        if (i > 15)
        {
            sprintf(cadena, "%.5f", data[i]);
        }
        else
        {
            sprintf(cadena, "%.3f", data[i]);
        }

        LoRa.printf("[%d]:%s\n", i, cadena);
    }

    // sprintf(cadena, "%.1f", data[0]);
    // LoRa.printf("(mq7Pin): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[1]);
    // LoRa.printf("(mq7VoltageValue): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[2]);
    // LoRa.printf("(mq7COppm): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[3]);
    // LoRa.printf("(mAh): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[4]);
    // LoRa.printf("(vbat): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[5]);
    // LoRa.printf("(vaps): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[6]);
    // LoRa.printf("(icharge): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[7]);
    // LoRa.printf("(idischarge): %s\n", cadena);
    // sprintf(cadena, "%.3f", data[8]);
    // LoRa.printf("(tempAXP192): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[9]);
    // LoRa.printf("(dhtHum): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[10]);
    // LoRa.printf("(dhtTemp): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[11]);
    // LoRa.printf("(dhtTempF): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[12]);
    // LoRa.printf("(bmpPres): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[13]);
    // LoRa.printf("(bmpTemp): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[14]);
    // LoRa.printf("(bmpAlti): %s\n", cadena);
    // sprintf(cadena, "%.2f", data[15]);
    // LoRa.printf("(TFdistance): %s\n", cadena);
    // sprintf(cadena, "%.1f", data[16]);
    // LoRa.printf("(TFstrength): %s\n", cadena);
    // sprintf(cadena, "%.7f", data[17]);
    // LoRa.printf("(gpsLatitude): %s\n", cadena);
    // sprintf(cadena, "%.7f", data[18]);
    // LoRa.printf("(gpsLongitude): %s\n", cadena);
    // sprintf(cadena, "%.7f", data[19]);
    // LoRa.printf("(gpsAltitude): %s\n", cadena);
    // Serial.println("");
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
