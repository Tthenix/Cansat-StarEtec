#ifndef LoraService_hpp
#define LoraService_hpp

#include <Arduino.h>
#include <LoRa.h>

class LoraService {
public:
    LoraService();
    void begin();
    bool sendSensorData(const float* data, size_t size);
    bool receiveConfirmation();
    bool reconnect();

private:
    // Agrega aquí las variables y constantes necesarias para la comunicación LoRa.
};

#endif // LoraService_h
