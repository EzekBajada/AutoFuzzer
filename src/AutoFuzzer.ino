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
void ClickHandler(uint8_t buttonCode)
{
    switch(buttonCode)
    {
        case 1:
        case 2:
        case 3:     
    }
}

GUI gui;
CANFuzzer fuzzer;
GUILabel titleLabel(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 16, "Auto Fuzzer", 2, 0xFFFF, true, NULL, 0);
GUILabel statusLabel(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, "Initialising...", 1, ILI9341_GREEN, true, NULL, 0);
GUILabel buttonLabelModeSpy(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, "Spy Mode", 1, ILI9341_GREEN, true, NULL, 0);
GUILabel buttonLabelModeFuzzing(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, "Fuzzing Mode", 1, ILI9341_GREEN, true, NULL, 0);
GUILabel buttonLabelModeIntelligent(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, "Intelligent Mode", 1, ILI9341_GREEN, true, NULL, 0);

// Buttons
GUIImage spyMode(gui.GetTFT(), gui.GetTS(),0,34,100,50,&buttonLabelModeSpy,&ClickHandler,1);      
GUIImage fuzzingMode(gui.GetTFT(), gui.GetTS(),0,34,100,50,&buttonLabelModeFuzzing,&ClickHandler,2);      
GUIImage intelligentFuzzerMode(gui.GetTFT(), gui.GetTS(),0,34,100,50,&buttonLabelModeIntelligent,&ClickHandler,2);      
void setup() 
{
    system_update_cpu_freq(160);
    
    fuzzer.statusLabel = &statusLabel;
    
    gui.RegisterElement(&titleLabel);
    gui.RegisterElement(&statusLabel);
    gui.Run();
    
    Serial.begin(250000);
    if(fuzzer.Init() == 0){ 
       fuzzer.AutoDetectCANSpeed();
    };
    
}

void loop() 
{
    gui.Run();
    fuzzer.Run();     
    yield();
}
