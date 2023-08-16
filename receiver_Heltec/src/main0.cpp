//
// Heltec 32 LoRa OLED (with SD Card)
//               ______________
//          Gnd |    |  USB|   | Gnd
//           5v |    | port|   |  5v
//          3v3 |    |_____|   | 3v3
//          Gnd |      ALL     |  36<-     <- : Input Only!
//           Rx |     GPIO     |  37<-
//           Tx |     3.3v     |  38<-
//          RST |   ________   |  39<-
//            0 |  |        |  |  34<-
//    (SCL)  22 |  |        |  |  35<-
// SPI MISO  19 |  |        |  |  32<-
//           23 |  |        |  |  33<-
//LoRa CS(HI)18 |  |        |  |  25  (LED)
// SPI SCK    5 |  |  OLED  |  |  26  LoRa IRQ
//OLED SCL   15 |  |        |  |  27  SPI MOSI
//            2 |  |        |  |  14  LoRa Rst
//OLED SCA    4 |  |        |  |  12
//           17 |  |________|  |  13
//OLED RST   16 |______________|  21  (SD_CS)
//

#include <mySD.h>        //  SD Card (VMA304)
#include "SSD1306.h"     //  OLED
#include <LoRa.h>        //  LoRa

File root;

//       OLED       Pin
#define  SDA          4   //  Serial Data
#define  SCL         15   //  Serial Clock
#define  oRST        16   //  Reset
#define  OLED_ADDR 0x3C   //  OLED display TWI address

SSD1306         display(OLED_ADDR, SDA, SCL);
//  OLED screen text rows:
#define  row1     0     //  y=0 is top row of size 1 text
#define  row2    10
#define  row3    20
#define  row4    30
#define  row5    40
#define  row6    50     //  row7 at 70 is too low

//  SPI port #2:  SD Card Adapter
#define  SD_CLK      5
#define  SD_MISO    19
#define  SD_MOSI    27
#define  SD_CS      21

//  SPI port:  LoRa (SX1278) 
//       LoRa      Pin
#define  LoRa_SCK    5
#define  LoRa_MISO  19
#define  LoRa_MOSI  27
#define  LoRa_CS    18
#define  LoRa_RST   14    //  LoRa_Reset
#define  DI0        26    //  LoRa_IRQ
#define  BAND    915E6    //  other freq: 433E6  // or ? LoRa f(MHz)

#define  Select    LOW   //  Low CS means that SPI device Selected
#define  DeSelect  HIGH  //  High CS means that SPI device Deselected

File     sessionFile;   //  SD card filenames are restricted to 8 characters + extension

int counter = 0;        //  count sent LoRa messages

/***********************************************************/
void setup(){  
  // set output pins
  pinMode(oRST,OUTPUT);
  pinMode(SD_CS,OUTPUT);
  pinMode(LoRa_CS,OUTPUT);
  digitalWrite(LoRa_CS, DeSelect);

  // set GPIO16 Low then High to Reset OLED
  digitalWrite(oRST, LOW);  
  delay(50);
  digitalWrite(oRST, HIGH);

  Serial.begin(115200);
  while(!Serial);                     //  wait to connect to computer

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.clear();
  display.display();

  Serial.print("Initializing SD card...");
  digitalWrite(SD_CS, Select);    //  SELECT (Low) SD Card SPI
  if (!SD.begin( SD_CS, SD_MOSI, SD_MISO, SD_CLK )) {
    Serial.println("initialization failed!");
    //  now what?
  } else {
    Serial.println("initialization done.");
    display.drawString( 5,row1,"SD Card OK!" );
    display.display();
    delay(1000);
  }
  /* open "test.txt" for writing */
  root = SD.open("test.txt", FILE_WRITE);
  if (root) {
    root.println("Hello world!");
    root.flush();
    root.close();
  } else {    //  file open error
    Serial.println("error opening test.txt");
  }
  display.drawString( 5,row2,"Wrote in test.txt" );
  display.display();
  delay(100);
  /* after writing, then reopen the file and read it */
  root = SD.open("test.txt");
  if (root) {    /* read from the file to the end of it */
    while (root.available()) {  // read the file
      Serial.write(root.read());
    }
    root.close();
  } else {
    Serial.println("error opening test.txt");
  }
  display.drawString( 5,row3,"Read from test.txt" );
  display.display();
  delay(100);
  //  done testing the SD Card
  digitalWrite(SD_CS, DeSelect);    //  DESELECT (high) SD Card SPI
  SD.end();
  delay( 100 ); 

  //  now test the LoRa

  SPI.begin( LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_CS );

  LoRa.setPins( LoRa_CS, LoRa_RST, DI0 );
  digitalWrite(LoRa_CS, Select);   //  SELECT (low) LoRa SPI 
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    display.drawString( 5, row4, "LoRa Init Failed!");
    display.display();
    // now what?
  } else {
    Serial.println("LoRa Initial OK!");
    display.drawString( 5, row4, "LoRa Initialized OK!");
    display.display();
    delay(1000);
  }
  Serial.println("Setup done!");
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 5, "Sending packet ");
  display.drawString(50, 30, String(counter));
  display.display();
  // send packets
  LoRa.beginPacket();
  LoRa.print("Hi. Sent packet #");
  LoRa.print(counter);
  LoRa.endPacket();
  counter++;
  delay(500);
}