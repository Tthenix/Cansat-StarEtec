#ifndef SENSOR_SERVICIO_H
#define SENSOR_SERVICIO_H

#include <M5StickC.h>
#include "DHT.h"

#define DHTPIN 25
#define DHTTYPE DHT11

class SensorServicio {
public:
    SensorServicio();

    void begin();
    void leerSensores();
    void mostrarValores();
    float getHumedad() const { return humedad; }
    float getTemperatura() const { return temperatura; }
    float getTemperaturaF() const { return temperaturaF; }
    float getVbat() const { return vbat; }
    float getVaps() const { return vaps; }
    float getIcharge() const { return icharge; }
    float getIdischarge() const { return idischarge; }
    float getTempAXP192() const { return tempAXP192; }

private:
    DHT dht;
    float humedad;
    float temperatura;
    float temperaturaF;
    float vbat;
    float mq7data;
    float vaps;
    float icharge;
    float idischarge;
    float tempAXP192;
};

#endif // SENSOR_SERVICIO_H
