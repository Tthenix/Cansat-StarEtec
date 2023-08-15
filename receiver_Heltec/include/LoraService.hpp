#ifndef LoraService_hpp
#define LoraService_hpp

#include <SPI.h>
#include <Arduino.h>
#include <LoRa.h>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26

#define BAND 866E6
class LoraService
{
public:
    LoraService();
    void begin();
    bool sendSensorData(const float *data, size_t size);
    void enviarDatosLoRa(float* data, int dataSize);
    bool receiveConfirmation();
    bool reconnect();

private:
    // Agrega aquí las variables y constantes necesarias para la comunicación LoRa.
    int counter; // Declaración del contador como miembro de la clase
};

#endif // LoraService_h
