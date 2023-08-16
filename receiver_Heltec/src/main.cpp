#include <Arduino.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define el sd
#include <SD.h>
File dataFile;

//  SPI port #2:  SD  Card Adapter
#define  SD_CLK      5
#define  SD_MISO    19
#define  SD_MOSI    27
#define  SD_CS      12

//  SPI port:  LoRa (SX1278) 
//       LoRa      Pin
#define  LoRa_SCK    5
#define  LoRa_MISO  19
#define  LoRa_MOSI  27
#define  LoRa_CS    18
#define  LoRa_RST   14    //  LoRa_Reset
#define  LoRa_DIO0   26    //  LoRa_IRQ


//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6


//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//  OLED screen text rows:
#define  row1     0     //  y=0 is top row of size 1 text
#define  row2    10
#define  row3    20
#define  row4    30
#define  row5    40
#define  row6    50     //  row7 at 70 is too low

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

#define  Select    LOW   //  Low CS means that SPI device Selected
#define  DeSelect  HIGH  //  High CS means that SPI device Deselected

void setup() { 
  //Pines ChipSelect de los perifericos
  pinMode(SD_CS, OUTPUT);
  pinMode(LoRa_CS, OUTPUT);
  digitalWrite(LoRa_CS, DeSelect);
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
 
  Serial.begin(115200);
  while(!Serial);                     //  wait to connect to computer

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();

  display.clearDisplay();
  display.setTextColor(WHITE);

  // // Inicializar SD
  // SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);  // Inicializar SPI para SD
  
  // // Inicializar la tarjeta microSD
  // digitalWrite(SD_CS, Select);    //  SELECT (Low) SD Card SPI
  // if (!SD.begin(SD_CS)) {
  //   Serial.println("Error al inicializar la tarjeta microSD.");
  //   display.setCursor(0,0);
  //   display.print("[ERROR] init SD");
  //   return;
  // }
  
  // Serial.println("Tarjeta microSD inicializada correctamente.");
  // // Verificar si el archivo ya existe, si no, crearlo con los encabezados
  // display.setCursor(0,0);
  // display.print("Módulo SD");
  // display.setCursor(0,20);
  // const char* fileName = "/datos.csv";
  // if (!SD.exists("datos.csv")) {
  //   File dataFile = SD.open("datos.csv", FILE_WRITE);
  //   if (dataFile) {
  //     dataFile.println("pin,humedad,temp1,corriente,temp2"); // Encabezados
  //     dataFile.close();
  //     Serial.println("Archivo datos.csv creado con encabezados.");
  //     display.print("Se ha creado: datos.csv");
  //   } else {
  //     Serial.println("Error al crear el archivo datos.csv.");
  //     display.print("[Error] crear: datos.csv");
  //   }
  // } else {
  //   Serial.print("el archivo ya existe");
  //   display.print("Tiene archivo datos.csv");
  // }
  // display.display();  
  // digitalWrite(SD_CS, DeSelect);    //  DESELECT (high) SD Card SPI
  // SPI.end();

  delay(1500);

  Serial.println("LoRa Receiver Test");
 
  //SPI LoRa pins
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_CS);
  //setup LoRa transceiver module
  LoRa.setPins(LoRa_CS, LoRa_RST, LoRa_DIO0);
  digitalWrite(LoRa_CS, Select);   //  SELECT (low) LoRa SPI 
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("LoRa Init OK!");
  display.display();  
  SPI.end();
  
}

void loop() {

  //try to parse packet
  //  use the LoRa
  SPI.begin( LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_CS );
  LoRa.setPins( LoRa_CS, LoRa_RST, LoRa_DIO0);
  digitalWrite(SD_CS, DeSelect);    //  deselet SD Card SPI
  digitalWrite(LoRa_CS, Select);    //  SELECT (low) LoRa SPI 

  int packetSize = LoRa.parsePacket();
  bool dataRead = true;

  if (packetSize) {
    dataRead = false;
    //received a packet
    Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);

    // Dsiplay information
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Cansat StarETec");
    display.setCursor(0,20);
    display.print("Mensaje recibido:");
    display.setCursor(0,30);
    display.print(LoRaData);
    display.setCursor(0,40);
    display.print("RSSI:");
    display.setCursor(30,40);
    display.print(rssi);
    display.display();  
    
    digitalWrite(LoRa_CS, DeSelect);    //  deselet SD Card SPI
    SPI.end();
  }

  // if(!dataRead){
  //   SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);  // Inicializar SPI para SD    
  //   // Inicializar la tarjeta microSD
  //   digitalWrite(SD_CS, Select);    //  SELECT (Low) SD Card SPI
  //   if (!SD.begin(SD_CS)){
  //     // Procesar el string y reemplazar '\n' con comas
  //     LoRaData.replace("\n", ",");

  //     // Abrir el archivo en modo anexar
  //     File dataFile = SD.open("datos.csv", FILE_APPEND);
  //     if (dataFile) {
  //       dataFile.println(LoRaData);
  //       dataFile.close();
  //       Serial.println("Datos escritos en 'datos.csv'.");
  //     } else {
  //       Serial.println("Error al abrir el archivo 'datos.csv'.");
  //     }

  //   }
  //   //  done using the SD Card
  //   digitalWrite(SD_CS, DeSelect);    //  DESELECT (high) SD Card SPI
  //   SD.end();
  //   dataRead = true;
  // }
  delay(500);
}
