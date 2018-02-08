/*
 * Teensy 3.6 - CAN 2.0B and CAN FD Demo Board
 * 
 * http://skpang.co.uk/catalog/teensy-36-can-20b-and-can-fd-demo-board-p-1551.html
 * 
 * v1.0 Feb 2018
 * 
 * skpang.co.uk
 * 
 */

#include "drv_canfdspi_api.h"
#include "drv_canfdspi_register.h"
#include "drv_canfdspi_defines.h"
#include "drv_spi.h"

#include <SPIN.h>
#include "SPI.h"
#include "ILI9341_t3n.h"
#include "ili9341_t3n_font_Arial.h"

#define SPI1_DISP

#ifdef SPI1_DISP

#define TFT_DC 6 // 0xe4
#define TFT_CS 31 // 0xe5
#define TFT_SCK 32 //0xe2
#define TFT_MISO 1 // 0xe3
#define TFT_MOSI 0 // 0xe1
#define TFT_RESET 8
ILI9341_t3n tft = ILI9341_t3n(TFT_CS, TFT_DC, TFT_RESET, TFT_MOSI, TFT_SCK, TFT_MISO, &SPIN1);
#endif

volatile uint32_t can_msg_count = 0;

extern uint8_t txd[MAX_DATA_BYTES];
extern CAN_TX_MSGOBJ txObj;

const int JOY_UP = 22; 
const int JOY_CLICK = 17; 
const int JOY_DOWN = 23; 
const int JOY_LEFT = 20; 
const int JOY_RIGHT = 21; 

void setup() {

  pinMode(JOY_UP, INPUT_PULLUP);
  pinMode(JOY_CLICK, INPUT_PULLUP);
  pinMode(JOY_DOWN, INPUT_PULLUP);
  pinMode(JOY_LEFT, INPUT_PULLUP);
  pinMode(JOY_RIGHT, INPUT_PULLUP);
  
  delay(1000);
  
   tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setFont(Arial_10);
  tft.setCursor(0, 0);
  tft.println("CAN FD Tx Test v1.0    skpang.co.uk 01/18");
  tft.setCursor(0, 20);
  tft.println("Click:Start/Pause");
  tft.setCursor(0, 31);
  tft.println("Up:Down  +/- delay ");
  tft.drawLine(0,46,340,46,ILI9341_DARKGREY);    //Horizontal line
  tft.setFont(Arial_10);
  tft.setCursor(0, 100);
  tft.println("Interval "); 
  
  Serial.println(F("CAN Bus Tx test"));
 
  APP_CANFDSPI_Init(CAN_500K_2M);       // 500k arbitration rate and 2Mbps data rate
  // APP_CANFDSPI_Init(CAN_500K_4M);       // 500k arbitration rate and 2Mbps data rate
  // APP_CANFDSPI_Init(CAN_500K_8M);       // 500k arbitration rate and 2Mbps data rate
 
  
  txObj.bF.ctrl.IDE = 0;      // Extended CAN ID false
  txObj.bF.id.SID = 0x7df;    // CAN ID
  txObj.bF.ctrl.BRS = 1;      // Switch Bitrate true (switch to 2Mbps)
  txObj.bF.ctrl.FDF = 1;      // CAN FD true
  txObj.bF.ctrl.DLC = 15;      // Data length, 64 bytes
  txd[4] = can_msg_count >> 24;
  txd[5] = can_msg_count >> 16;
  txd[6] = can_msg_count >> 8;
  txd[7] = can_msg_count;
  delay(100);
     
   
}
void update_count(void)
{ 
    tft.fillRect(60,125,100,15,ILI9341_BLACK); // Clear count area
    tft.setCursor(60, 125);
    tft.println(can_msg_count);

    tft.fillRect(60,150,235,15,ILI9341_BLACK); // Clear count area

    tft.setCursor(60, 150);
    tft.print("00");

    tft.setCursor(85, 150);
    tft.print("00");

    tft.setCursor(110, 150);
    tft.print("00");

    tft.setCursor(135, 150);
    tft.print("00");
    
    tft.setCursor(160, 150);
    tft.print((can_msg_count >> 24),HEX);
 
    tft.setCursor(185, 150);
    tft.print((can_msg_count >> 16)&0xff,HEX);
    
    tft.setCursor(210, 150);
    tft.print((can_msg_count >> 8)&0xff,HEX);

    tft.setCursor(235, 150);
    tft.print(can_msg_count&0xff,HEX);
}


void loop() {
  uint8_t i;
  
  if(digitalRead(JOY_CLICK) == 0)
  {
     Serial.println(F("Click"));
     delay(10);
  }
    
  if(digitalRead(JOY_UP) == 0)
  {
     Serial.println(F("up"));
     delay(50);
  }

  if(digitalRead(JOY_RIGHT) == 0)
  {
     Serial.println(F("RIGHT"));
     delay(50);
  }

  if(digitalRead(JOY_LEFT) == 0)
  {
     Serial.println(F("LEFT"));
     delay(50);
  }
  
  if(digitalRead(JOY_DOWN) == 0)
  {
     Serial.println(F("DOWN"));
     delay(50);
  }
  
  txd[4] = can_msg_count >> 24;
  txd[5] = can_msg_count >> 16;
  txd[6] = can_msg_count >> 8;
  txd[7] = can_msg_count;
  
  APP_TransmitMessageQueue(); // Send out CAN FD frame

  can_msg_count++;
  update_count();
  delay(100);

  if(digitalReadFast(int1_pin) == 0)  // Read int1 pin on MCP2517FD
  {
    // It is low read out the data
    APP_ReceiveMessage_Tasks();
  }

}






