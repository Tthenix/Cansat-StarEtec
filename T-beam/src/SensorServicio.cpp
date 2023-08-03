#include "../include/SensorServicio.hpp"

SensorServicio::SensorServicio() : dht(DHTPIN, DHTTYPE) {}

void SensorServicio::begin() {
    M5.begin();
    Serial.begin(115200);
    dht.begin();
}

void SensorServicio::leerSensores() {
    int adc_MQ = analogRead(14);
    mq7data = adc_MQ * (5.0 / 1023.0);

    vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
    vaps = M5.Axp.GetVapsData() * 1.4 / 1000;
    icharge = M5.Axp.GetIchargeData() / 2.0;
    idischarge = M5.Axp.GetIdischargeData() / 2.0;
    tempAXP192 = M5.Axp.GetTempInAXP192();

    humedad = dht.readHumidity();
    temperatura = dht.readTemperature();
    temperaturaF = dht.readTemperature(true);
}

void SensorServicio::mostrarValores() {
    Serial.print("adc:");
    Serial.print(analogRead(14));
    Serial.print("    voltaje:");
    Serial.println(mq7data);

    Serial.print("Vbat: ");
    Serial.print(vbat, 3);
    Serial.println(" V");

    Serial.print("Vaps: ");
    Serial.print(vaps, 3);
    Serial.println(" V");

    Serial.print("Icharge: ");
    Serial.print(icharge, 3);
    Serial.println(" mA");

    Serial.print("Idischarge: ");
    Serial.print(idischarge, 3);
    Serial.println(" mA");

    Serial.print("Temperatura: ");
    Serial.print(tempAXP192);
    Serial.println(" °C");

    Serial.println("-------------------");

    Serial.print("Humedad ");
    Serial.print(humedad);
    Serial.print(" %t");
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" *C ");
    Serial.print(temperaturaF);
    Serial.println(" *F");
}
