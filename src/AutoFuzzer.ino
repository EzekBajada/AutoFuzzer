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
void LabelClick(uint8_t boxCode);

uint32_t sessionID = 1;
CANFuzzerInputs input = SnifferFile;
CANFuzzerModes mode = Manual;
uint32_t FuzzedID = 0xAA;
uint8_t FuzzedBytes = 0b00011000;
 
CANSniffer sniffer;
CANFuzzer fuzzer;
GUI gui;
SDCard sdc;
// Always on screen - 0
GUILabel titleLabel(gui.GetTFT(), gui.GetTS(), 0, 0, 320, 16, 0, "Auto Fuzzer", 2, 0xFFFF, true, NULL, 0, NULL, 0, 0);
GUILabel statusLabel(gui.GetTFT(), gui.GetTS(), 0, 17, 320, 8, 0, "Initialising...", 1, ILI9341_GREEN, true, NULL, 0, NULL, 0, 0);
GUILabel settingsLabel(gui.GetTFT(), gui.GetTS(), 5, 10, 50, 50, 0, "Settings", 1, ILI9341_RED, false, LabelClick, 2, NULL, 0, 0);

// Interface Buttons Screen - 1
GUIImage snifferIcon(gui.GetTFT(), gui.GetTS(), 30, 30, 50, 50, 1, SnifferImage, SnifferImageLength, IconClick, 1);
GUILabel snifferText(gui.GetTFT(), gui.GetTS(), 30, 80, 320, 8, 1, "Sniffer", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage fuzzingIcon(gui.GetTFT(), gui.GetTS(), 30, 90, 50, 50, 1, NormalFuzzerImage, NormalFuzzerImageLength, IconClick, 2);
GUILabel fuzzingText(gui.GetTFT(), gui.GetTS(), 30, 140, 320, 8, 1, "Analyze", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUIImage manInMiddleIcon(gui.GetTFT(), gui.GetTS(), 30, 150, 50, 50, 1, IntelligentFuzzerImage, IntelligentFuzzerImageLength, IconClick, 3);
GUILabel manInMiddeText(gui.GetTFT(), gui.GetTS(), 30, 200, 320, 8, 1, "Fuzz!", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);

//Settings Screen - 4
GUILabel sessionLabel(gui.GetTFT(), gui.GetTS(), 0, 30, 320, 20, 4, "Session ID", 1, ILI9341_WHITE, true, NULL, 0, NULL, 0, 0);
GUINumScroll numScroll(gui.GetTFT(), gui.GetTS(), 30, 50, 20, 20, 4, 0);
GUINumScroll numScroll2(gui.GetTFT(), gui.GetTS(), 70, 50, 20, 20, 4, 0);
GUINumScroll numScroll3(gui.GetTFT(), gui.GetTS(), 110, 50, 20, 20, 4, 0);

GUILabel bytesLabel(gui.GetTFT(), gui.GetTS(), 0, 80, 320, 20, 4, "Bytes", 1, ILI9341_WHITE, true, NULL, 0, NULL, 0, 0);
GUICheckBox boxForByte1(gui.GetTFT(), gui.GetTS(), 10, 105, 20, 20, 4, BoxClick, 3);
GUICheckBox boxForByte2(gui.GetTFT(), gui.GetTS(), 40, 105, 20, 20, 4, BoxClick, 4);
GUICheckBox boxForByte3(gui.GetTFT(), gui.GetTS(), 70, 105, 20, 20, 4, BoxClick, 5);
GUICheckBox boxForByte4(gui.GetTFT(), gui.GetTS(), 100, 105, 20, 20, 4, BoxClick, 6);
GUICheckBox boxForByte5(gui.GetTFT(), gui.GetTS(), 130, 105, 20, 20, 4, BoxClick, 7);
GUICheckBox boxForByte6(gui.GetTFT(), gui.GetTS(), 160, 105, 20, 20, 4, BoxClick, 8);
GUICheckBox boxForByte7(gui.GetTFT(), gui.GetTS(), 190, 105, 20, 20, 4, BoxClick, 9);
GUICheckBox boxForByte8(gui.GetTFT(), gui.GetTS(), 220, 105, 20, 20, 4, BoxClick, 10);
GUILabel byte1Label(gui.GetTFT(), gui.GetTS(), 10, 135, 20, 20, 4, "B 1", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte2Label(gui.GetTFT(), gui.GetTS(), 40, 135, 20, 20, 4, "B 2", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte3Label(gui.GetTFT(), gui.GetTS(), 70, 135, 20, 20, 4, "B 3", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte4Label(gui.GetTFT(), gui.GetTS(), 100, 135, 20, 20, 4, "B 4", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte5Label(gui.GetTFT(), gui.GetTS(), 130, 135, 20, 20, 4, "B 5", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte6Label(gui.GetTFT(), gui.GetTS(), 160, 135, 20, 20, 4, "B 6", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte7Label(gui.GetTFT(), gui.GetTS(), 190, 135, 20, 20, 4, "B 7", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUILabel byte8Label(gui.GetTFT(), gui.GetTS(), 220, 135, 20, 20, 4, "B 8", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);

GUILabel inputLabel(gui.GetTFT(), gui.GetTS(), 10, 175, 20, 20, 4, "Inputs: ", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUICheckBox boxInput1(gui.GetTFT(), gui.GetTS(), 60, 175, 20, 20, 4, BoxClick, 11);
GUILabel snifferFileLabel(gui.GetTFT(), gui.GetTS(), 90, 175, 20, 20, 4, "Sniffer File", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUICheckBox boxInput2(gui.GetTFT(), gui.GetTS(), 180, 175, 20, 20, 4, BoxClick, 12);
GUILabel liveLabel(gui.GetTFT(), gui.GetTS(), 210, 175, 20, 20, 4, "Live", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);

GUILabel modeLabel(gui.GetTFT(), gui.GetTS(), 10, 215, 20, 20, 4, "Modes: ", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUICheckBox boxMode1(gui.GetTFT(), gui.GetTS(), 60, 215, 20, 20, 4, BoxClick, 13);
GUILabel noneLabel(gui.GetTFT(), gui.GetTS(), 90, 215, 20, 20, 4, "None", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUICheckBox boxMode2(gui.GetTFT(), gui.GetTS(), 120, 215, 20, 20, 4, BoxClick, 14);
GUILabel manualLabel(gui.GetTFT(), gui.GetTS(), 150, 215, 20, 20, 4, "Manual", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);
GUICheckBox boxMode3(gui.GetTFT(), gui.GetTS(), 190, 215, 20, 20, 4, BoxClick, 15);
GUILabel autoLabel(gui.GetTFT(), gui.GetTS(), 220, 215, 20, 20, 4, "Automatic", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);

//GUICheckBox okBox(gui.GetTFT(), gui.GetTS(), 300, 210, 20, 20, 4, BoxClick, 1);
GUILabel okLabel(gui.GetTFT(), gui.GetTS(), 260, 160, 30, 30, 4, "OK", 3, ILI9341_WHITE, false, LabelClick, 1, NULL, 0, 0);
GUILabel scrollText(gui.GetTFT(), gui.GetTS(), 0, 30, 320, 50, 10, "", 1, ILI9341_WHITE, false, NULL, 0, NULL, 0, 0);


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
                fuzzer.FuzzedID = 0;
                fuzzer.FuzzedBytes = 0;  
                fuzzer.Start(sessionID, Analyse, input);
            }            
        }
        break;
        case 3:
        {
            if (fuzzer.GetEnabled())
            {
                fuzzer.Stop(); 
            }
            else
            {
                fuzzer.FuzzedID = FuzzedID;
                fuzzer.FuzzedBytes = FuzzedBytes;                
                fuzzer.Start(sessionID, mode, input);
            }   
        }
        break;
    }  
}

void BoxClick(uint8_t boxCode) 
{
  switch(boxCode)
  {
    case 1: //okBox
    {
       //okBox.inClickHandler = true;
       uint8_t y = 0;
       for(uint16_t i = 0; i < gui.GetElementsCount(); i++) if(gui.GetElements()[i]->ScreenNumber == 0) y += gui.GetElements()[i]->Y;
       gui.GetTFT()->fillRect(0,y,320,240,ILI9341_BLACK);
       gui.ScreenNumber = 1; 
    }
    break;  
    case 2: //Session id 
    {
      String numString = String(numScroll.CurrNum) + String(numScroll2.CurrNum) + String(numScroll3.CurrNum);
       sessionID = numString.toInt();
       Serial.println(sessionID);
    }
    break;
    case 3: 
    {
      FuzzedBytes >> 1;
    }
    break;
    case 11:
    { 
        boxInput2.BoxClickedInProgress = false;
        boxInput2.Redraw();
    }
    break;
    case 12:
    {
        boxInput1.BoxClickedInProgress = false;
        boxInput1.Redraw();
    }
    break;
    case 13:
    {
        boxMode2.BoxClickedInProgress = false;
        boxMode3.BoxClickedInProgress = false;
        boxMode2.Redraw();
        boxMode3.Redraw();
    }
    break;
    case 14:
    {
        boxMode1.BoxClickedInProgress = false;
        boxMode3.BoxClickedInProgress = false;
        boxMode1.Redraw();
        boxMode3.Redraw();
    }
    break;
    case 15:
    {
        boxMode1.BoxClickedInProgress = false;
        boxMode2.BoxClickedInProgress = false;
        boxMode1.Redraw();
        boxMode2.Redraw();
    }
    break;
  }
}

void LabelClick(uint8_t boxCode) 
{
    switch(boxCode)
    {
        case 1: //okBox
        {
            Serial.println("INAFSET");
            uint8_t y = 0;
            for(uint16_t i = 0; i < gui.GetElementsCount(); i++) if(gui.GetElements()[i]->ScreenNumber == 0) y += gui.GetElements()[i]->Y;
            gui.GetTFT()->fillRect(0,y,320,240,ILI9341_BLACK);
            for(uint16_t i = 0; i < gui.GetElementsCount(); i++) if(gui.GetElements()[i]->ScreenNumber == 1) gui.GetElements()[i]->Redraw();
            gui.ScreenNumber = 1;
            FuzzedBytes = 0;
            if((String(numScroll.CurrNum) + String(numScroll2.CurrNum) + String(numScroll3.CurrNum)) != 0)
            {
                sessionID = (String(numScroll.CurrNum) + String(numScroll2.CurrNum) + String(numScroll3.CurrNum)).toInt();
            }
            if (boxForByte1.BoxClickedInProgress) FuzzedBytes |= 0b10000000;
            if (boxForByte2.BoxClickedInProgress) FuzzedBytes |= 0b01000000;
            if (boxForByte3.BoxClickedInProgress) FuzzedBytes |= 0b00100000;
            if (boxForByte4.BoxClickedInProgress) FuzzedBytes |= 0b00010000;
            if (boxForByte5.BoxClickedInProgress) FuzzedBytes |= 0b00001000;
            if (boxForByte6.BoxClickedInProgress) FuzzedBytes |= 0b00000100;
            if (boxForByte7.BoxClickedInProgress) FuzzedBytes |= 0b00000010;
            if (boxForByte8.BoxClickedInProgress) FuzzedBytes |= 0b00000001;
            if (boxMode1.BoxClickedInProgress) mode = None;
            if (boxMode2.BoxClickedInProgress) mode = Manual;
            if (boxMode3.BoxClickedInProgress) mode = Automatic;
            
            if (boxInput1.BoxClickedInProgress) input = SnifferFile;
            if (boxInput2.BoxClickedInProgress) input = LiveCapture;
//            // CAN ID setting goes here

            // INput setting goes here
            Serial.println(sessionID);
        }
        break;  
        case 2:
        {
          uint8_t y = 0;
          for(uint16_t i = 0; i < gui.GetElementsCount(); i++) if(gui.GetElements()[i]->ScreenNumber == 0) y += gui.GetElements()[i]->Y;
          gui.GetTFT()->fillRect(0,y,320,240,ILI9341_BLACK);
          for(uint16_t i = 0; i < gui.GetElementsCount(); i++) if(gui.GetElements()[i]->ScreenNumber == 4) gui.GetElements()[i]->Redraw();
          gui.ScreenNumber = 4;
        }
        break;
        case 3:
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
    gui.RegisterElement(&settingsLabel);
    gui.RegisterElement(&numScroll);
    gui.RegisterElement(&numScroll2);
    gui.RegisterElement(&numScroll3);
    gui.RegisterElement(&boxForByte1);
    gui.RegisterElement(&boxForByte2);
    gui.RegisterElement(&boxForByte3);
    gui.RegisterElement(&boxForByte4);
    gui.RegisterElement(&boxForByte5);
    gui.RegisterElement(&boxForByte6);
    gui.RegisterElement(&boxForByte7);
    gui.RegisterElement(&boxForByte8);
    gui.RegisterElement(&bytesLabel);
    gui.RegisterElement(&sessionLabel);
    gui.RegisterElement(&byte1Label);
    gui.RegisterElement(&byte2Label);
    gui.RegisterElement(&byte3Label);
    gui.RegisterElement(&byte4Label);
    gui.RegisterElement(&byte5Label);
    gui.RegisterElement(&byte6Label);
    gui.RegisterElement(&byte7Label);
    gui.RegisterElement(&byte8Label);
    gui.RegisterElement(&okLabel);
    gui.RegisterElement(&inputLabel);
    gui.RegisterElement(&boxInput1);
    gui.RegisterElement(&boxInput2);
    gui.RegisterElement(&modeLabel);
    gui.RegisterElement(&boxMode1);
    gui.RegisterElement(&boxMode2);
    gui.RegisterElement(&boxMode3);
    gui.RegisterElement(&noneLabel);
    gui.RegisterElement(&manualLabel);
    gui.RegisterElement(&autoLabel);
    gui.RegisterElement(&snifferFileLabel);
    gui.RegisterElement(&liveLabel);

    gui.Run();    
   
    if (fuzzer.Init() != 0) while(true) yield();
    sniffer.SetCANReceiver(fuzzer.GetCANReceiver());
    fuzzer.AutoDetectCANSpeed();
    gui.ScreenNumber = 11;
    timestart = millis();
}

void loop() 
{
    sniffer.Run();
    fuzzer.Run();
    gui.Run();
    yield();
}
