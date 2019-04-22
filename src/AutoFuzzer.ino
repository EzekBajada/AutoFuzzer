#include "AutoFuzzer.h"


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


GUI gui;
GUILabel titleLabel(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 16, "Auto Fuzzer", 2, 0xFFFF, true, NULL, 0);
GUILabel statusLabel(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, "Initialising...", 1, ILI9341_GREEN, true, NULL, 0);


CANFuzzer fuzzer;



void setup() 
{
    system_update_cpu_freq(160);
    
    fuzzer.statusLabel = &statusLabel;
    
    gui.RegisterElement(&titleLabel);
    gui.RegisterElement(&statusLabel);
    gui.Run();
    
    Serial.begin(250000);
    Serial.println("\n\nCAN-Bus Fuzzer\n");
    Serial.print("Initialising...");
    uint8_t result = fuzzer.Init();
    switch(result)
    {
        case 1: 
        {
            Serial.println("Error Initialising CAN RX!");
            break;
        }
        case 2:
        {
            Serial.println("Error Initialising CAN TX!");
            break;
        }
        default: 
        {
            Serial.println("Unknown error!");
            break;
        }
    }
    
    
    
    fuzzer.AutoDetectCANSpeed();
    
}

void loop() 
{
    gui.Run();
    fuzzer.Run();     
    yield();
}
