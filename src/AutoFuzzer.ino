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

void IconClick(uint8_t imageCode);
void BoxClick(uint8_t boxCode);

uint32_t sessionID = 1;
CANFuzzerInputs input = SnifferFile;
 
CANSniffer sniffer;
CANFuzzer fuzzer;
GUI gui;
// Number Scroll
GUINumScroll numScroll(gui.GetTFT(), gui.GetTS(), 30, 30, 20, 20, 4, 0);
GUINumScroll numScroll2(gui.GetTFT(), gui.GetTS(), 70, 30, 20, 20, 4, 0);
GUINumScroll numScroll3(gui.GetTFT(), gui.GetTS(), 110, 30, 20, 20, 4, 0);

SDCard sdc;

// Checkbox
GUICheckBox boxForNum(gui.GetTFT(), gui.GetTS(), 160, 30, 20, 20, 4, BoxClick, 1);

// Labels
GUILabel titleLabel(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 16, 0, "Auto Fuzzer", 2, 0xFFFF, true, NULL, 0, NULL, 0, 0);
GUILabel statusLabel(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, 0, "Initialising...", 1, ILI9341_GREEN, true, NULL, 0, NULL, 0, 0);
GUILabel scrollText(gui.GetTFT(), gui.GetTS(), 0, 30, 320, 50, 10, "", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel sessionLabel(gui.GetTFT(), gui.GetTS(), 190, 30, 50, 50, 10, "Session ID", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage settingsIcon(gui.GetTFT(), gui.GetTS(), 30, 30, 50, 50, 10, SnifferImage, SnifferImageLength, IconClick, 4);
// Buttons
GUIImage snifferIcon(gui.GetTFT(), gui.GetTS(), 30, 30, 50, 50, 1, SnifferImage, SnifferImageLength, IconClick, 1);
GUILabel snifferText(gui.GetTFT(), gui.GetTS(), 30, 80, 320, 8, 1, "Sniffer", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage fuzzingIcon(gui.GetTFT(), gui.GetTS(), 30, 90, 50, 50, 1, NormalFuzzerImage, NormalFuzzerImageLength, IconClick, 2);
GUILabel fuzzingText(gui.GetTFT(), gui.GetTS(), 30, 140, 320, 8, 1, "Analyze", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage manInMiddleIcon(gui.GetTFT(), gui.GetTS(), 30, 150, 50, 50, 1, IntelligentFuzzerImage, IntelligentFuzzerImageLength, IconClick, 3);
GUILabel manInMiddeText(gui.GetTFT(), gui.GetTS(), 30, 200, 320, 8, 1, "Fuzz!", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);


void IconClick(uint8_t imageCode)
{
    switch(imageCode)
    {
        case 1:
        {
            if (sniffer.GetEnabled())
            {
                sniffer.Stop(); 
            }
            else
            {
                sniffer.Start(sessionID); 
            }
        }
        break;
        case 2:
        {
            if (fuzzer.GetEnabled())
            {
                fuzzer.Stop(); 
            }
            else
            {
                fuzzer.Start(sessionID, Analyse, input);
            }            
        }
        break;
        case 3:
        {
          // Set the settings here for the fuzzer. 
          //fuzzer.Run();
        }
        break;
        case 4:
        {
      //    gui.GetTFT()->fillRect(30,30,50,50,ILI9341_BLACK);
      //    gui.ScreenNumber = 4;
        }
        break;
    }  
}

void BoxClick(uint8_t boxCode) 
{
  switch(boxCode)
  {
    case 1:
    {
    //  String wholeNum = String(numScroll.CurrNum) + String(numScroll2.CurrNum) + String(numScroll3.CurrNum);
    //  fuzzer.SetSessionID(wholeNum); 
    }
    break;  
    case 2:
    {
      
    }
    break;
  }
}





long timestart;

void setup() 
{
    system_update_cpu_freq(160);

    Serial.begin(250000);
    Serial.println("\n\nCAN-Bus Fuzzer\n");
    //Serial.print("Initialising...");


    gui.ScreenNumber = 9999999999;
    sniffer.statusLabel = &statusLabel;
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

    // Register the elements
    gui.RegisterElement(&titleLabel);
    gui.RegisterElement(&statusLabel);
    gui.RegisterElement(&scrollText);
    gui.RegisterElement(&snifferIcon);
    gui.RegisterElement(&snifferText);
    gui.RegisterElement(&fuzzingIcon);
    gui.RegisterElement(&fuzzingText);
    gui.RegisterElement(&manInMiddleIcon);
    gui.RegisterElement(&manInMiddeText);
    gui.RegisterElement(&settingsIcon);
    gui.RegisterElement(&boxForNum);
    gui.RegisterElement(&numScroll);
    gui.RegisterElement(&numScroll2);
    gui.RegisterElement(&numScroll3);
    gui.RegisterElement(&sessionLabel);
    
    gui.Run();    
   
    if (fuzzer.Init() != 0) while(true) yield();
    sniffer.SetCANReceiver(fuzzer.GetCANReceiver());
    fuzzer.AutoDetectCANSpeed();
    gui.ScreenNumber = 1;
    timestart = millis();
}

void loop() 
{
    sniffer.Run();
    fuzzer.Run();
    gui.Run();
    yield();
}
