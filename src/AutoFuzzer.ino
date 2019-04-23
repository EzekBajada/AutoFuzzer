#include "AutoFuzzer.h"
#include "Bitmaps.h"


/* PINS CAN TX  CAN RX  TFT   TOUCH   SD CARD
 * D0                   D/C
 * D1                                 CS
 * D2                         CS
 * D3           CS
 * D4   CS
 * D5   SCK     SCK     SCK   SCK     SCK
 * D6   MISO    MISO    MISO  MISO    MISO
 * D7   MOSI    MOSI    MOSI  MOSI    MOSI
 * D8                   CS   
 */

/* Screen Codes
 * 0 = 
 */


void IconClick(uint8_t imageCode)
{
    switch(imageCode)
    {
        case 1:
        {
           
        }
        break;
    }  
}

GUI gui;
GUILabel titleLabel(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 16, 0, "Auto Fuzzer", 2, 0xFFFF, true, NULL, 0);
GUILabel statusLabel(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, 0, "Initialising...", 1, ILI9341_GREEN, true, NULL, 0);
GUIImage snifferIcon(gui.GetTFT(), gui.GetTS(), 30, 30, 50, 50, 1, SnifferImage, SnifferImageLength, IconClick, 1);

CANFuzzer fuzzer;
CANSniffer sniffer;

void setup() 
{
    system_update_cpu_freq(160);

    gui.ScreenNumber = 9999999999;
    fuzzer.statusLabel = &statusLabel;    
    gui.RegisterElement(&titleLabel);
    gui.RegisterElement(&statusLabel);
    gui.RegisterElement(&snifferIcon);
    gui.Run();
    
    //Serial.begin(250000);
    //Serial.println("\n\nCAN-Bus Fuzzer\n");
    //Serial.print("Initialising...");
   
   if (fuzzer.Init() != 0) while(true) yield();
   sniffer.SetCANReceiver(fuzzer.GetCANReceiver());
   fuzzer.AutoDetectCANSpeed();
   gui.ScreenNumber = 1;
}

void loop() 
{
    gui.Run();
    sniffer.Run();
    fuzzer.Run();     
    yield();
}
