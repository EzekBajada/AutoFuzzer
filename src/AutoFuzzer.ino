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

void BoxClick() 
{
  
}
GUI gui;
//GUIScroll scroll(gui.GetTFT(),gui.GetTS(), 0,0,0,0, 1,lines);

// Labels
GUILabel titleLabel(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 16, 0, "Auto Fuzzer", 2, 0xFFFF, true, NULL, 0, NULL, 0, 0);
GUILabel statusLabel(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, 0, "Initialising...", 1, ILI9341_GREEN, true, NULL, 0, NULL, 0, 0);
GUILabel scrollText(gui.GetTFT(), gui.GetTS(), 0, 30, 320, 50, 1, "", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);

// Buttons
GUIImage snifferIcon(gui.GetTFT(), gui.GetTS(), 30, 30, 50, 50, 2, SnifferImage, SnifferImageLength, IconClick, 1);
GUILabel snifferText(gui.GetTFT(), gui.GetTS(), 30, 80, 320, 8, 2, "Sniffer", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage fuzzingIcon(gui.GetTFT(), gui.GetTS(), 30, 90, 50, 50, 2, NormalFuzzerImage, NormalFuzzerImageLength, IconClick, 1);
GUILabel fuzzingText(gui.GetTFT(), gui.GetTS(), 30, 140, 320, 8, 2, "Fuzzer", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage manInMiddleIcon(gui.GetTFT(), gui.GetTS(), 30, 150, 50, 50, 2, IntelligentFuzzerImage, IntelligentFuzzerImageLength, IconClick, 1);
GUILabel manInMiddeText(gui.GetTFT(), gui.GetTS(), 30, 200, 320, 8, 2, "Man In The Middle", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);

// Checkbox 
GUICheckBox box(gui.GetTFT(), gui.GetTS(), 30, 100, 50, 50, 3, BoxClick);

// Number Scroll
//GUINumScroll numScroll(gui.GetTFT(), gui.GetTS(), 30, 100, 50, 50, 4, 4);

CANFuzzer fuzzer;
CANSniffer sniffer;

long timestart;

void setup() 
{
    system_update_cpu_freq(160);

    Serial.begin(250000);
    Serial.println("\n\nCAN-Bus Fuzzer\n");
    //Serial.print("Initialising...");


    gui.ScreenNumber = 9999999999;
    fuzzer.statusLabel = &statusLabel;
 //   scroll.displayBlock = &scrollText;
    scrollText.LineCount = 7;
    scrollText.Lines = (String**) malloc(sizeof(String*) * scrollText.LineCount);
    scrollText.Lines[0] = new String("First Line");
    scrollText.Lines[1] = new String("Second Line");
    scrollText.Lines[2] = new String("Third Line");
    scrollText.Lines[3] = new String("Fourth Line");
    scrollText.Lines[4] = new String("Fifth Line");
    scrollText.Lines[5] = new String("Sixth Line");
    scrollText.Lines[6] = new String("Seventh Line");
    
    gui.RegisterElement(&titleLabel);
    gui.RegisterElement(&statusLabel);
    gui.RegisterElement(&scrollText);
    gui.RegisterElement(&snifferIcon);
    gui.RegisterElement(&snifferText);
    gui.RegisterElement(&fuzzingIcon);
    gui.RegisterElement(&fuzzingText);
    gui.RegisterElement(&manInMiddleIcon);
    gui.RegisterElement(&manInMiddeText);
    gui.RegisterElement(&box);
    // gui.RegisterElement(&numScroll);
    gui.Run();
    
   
   if (fuzzer.Init() != 0) while(true) yield();
   sniffer.SetCANReceiver(fuzzer.GetCANReceiver());
   fuzzer.AutoDetectCANSpeed();
   gui.ScreenNumber = 1;


   sniffer.Start(123);
   timestart = millis();
}

int i = 0;

void loop() 
{
    gui.Run();
    sniffer.Run();
    fuzzer.Run();     
    yield();


    i++;
    if (i == 10000)
    { 
        Serial.println("stopping sniffer");
        sniffer.Stop();
        Serial.println("time " + String(millis() - timestart));
        Serial.println("messages " + String(sniffer.testcounter));
    }
}
