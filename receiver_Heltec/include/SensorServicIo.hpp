#ifndef SENSOR_SERVICIO_H
#define SENSOR_SERVICIO_H

#include <Adafruit_BMP085.h>
#include <Wire.h>
#include <SPI.h>

// Paramatros a tener en cuenta BMP180

#define BMPSCL 22
#define BMPSDA 21

class SensorServicio{
    public:
        SensorServicio();

        void begin();
        void leerSensores();
        void mostrarValores();
        void formatearData(float *data);

        //*** BMP180 Getters ***/
        float getTemperaturaBMP () const { return bmpTemp;}
        float getPresionBMP () const { return bmpPres;}
        float getPresionNivelMarBMP () const { return bmpPressea;}
        float getAltitudBMP () const { return bmpAlt;}
        float getRealAltitudBMP () const { return bmpRealAlt;}

    private:
        //*** BMP180 values ***/
        Adafruit_BMP085 bmp;
        float bmpTemp;
        float bmpPres;
        float bmpPressea;
        float bmpAlt;
        float bmpRealAlt;
};

#endif