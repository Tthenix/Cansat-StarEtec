#include <Arduino.h>
#include <FS.h>
#include <esp_camera.h>
#include <SD_MMC.h> // Librería para manejar la tarjeta microSD
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory


// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Inicialización de la cámara
#define CAMERA_MODEL_AI_THINKER
#include "../include/camera_pins.h"

int pictureNumber = 0;

const int buttonPin = 2; // GPIO2

void tomarFoto();

bool inicializarSD() {
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return false;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return false;
  }

  return true;
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  if (!inicializarSD()) {
    return; // Detener el proceso si la inicialización de la tarjeta falla
  }

}

void loop() {
  // lastButtonState = reading;
  if (digitalRead(buttonPin)){
    Serial.println("botón sin presionar");
    delay(1000);
  } else if (!digitalRead(buttonPin)){
    Serial.println("botón presionado");
    delay(1500);
    tomarFoto();
  }
}

void tomarFoto(){
  
  camera_fb_t *fb = NULL;

  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;

  // Path donde se guardará la imagen en la SD Card
  String path = "/picture" + String(pictureNumber) + ".jpg";

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber); // Incrementar número de imagen y actualizar EEPROM
    EEPROM.commit();
  }
  file.close();
  esp_camera_fb_return(fb);

  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  // pinMode(4, OUTPUT);
  // digitalWrite(4, LOW);
  // delay(500);
  // rtc_gpio_hold_en(GPIO_NUM_4);

  delay(2000);
  Serial.println("Foto tomada");
}
