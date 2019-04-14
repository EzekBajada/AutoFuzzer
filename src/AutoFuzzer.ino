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
CANFuzzer fuzzer;

void setup() 
{
    system_update_cpu_freq(160);
    Serial.begin(250000);
    Serial.println("\n\nCAN-Bus Fuzzer\n");
    Serial.print("Initialising...");
    
    
    


}

void loop() 
{
    gui.Run();
    fuzzer.Run();     
    yield();

}
