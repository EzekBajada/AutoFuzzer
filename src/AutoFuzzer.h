#pragma once

#include <SPI.h>
#include <SD.h>
#include "mcp_can.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"
#include <string>


extern "C" 
{
    #include "user_interface.h"
}

class GUIElement
{
    public:
        GUIElement(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber);
        virtual ~GUIElement() {};
        Adafruit_ILI9341* tft;
        XPT2046_Touchscreen* touch;
        uint16_t X;
        uint16_t Y;
        uint16_t Width;
        uint16_t Height;
        uint16_t ScreenNumber;
        virtual void Run(TS_Point* clickPoint) = 0;

    protected:
        void fillArc(uint16_t x, uint16_t y, uint16_t start_angle, uint16_t end_angle, uint16_t radiusX, uint16_t radiusY, uint16_t width, uint16_t colour);
        uint16_t turnToGrayScale(uint16_t color);
};

class GUIImage: public GUIElement
{
    public:
        GUIImage(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t screenNumber, const uint8_t* Image, uint16_t ImageLength, void (*ClickHandler)(uint8_t imageCode), uint8_t callBackCode);
        ~GUIImage() {};
        const uint8_t* Image;
        uint16_t ImageLength;
        void Run(TS_Point* clickPoint);        

    private:
        TS_Point* clickStartPoint = NULL;
        bool needsRedrawing = true;
        void (*ClickHandler)(uint8_t imageCode);
        uint8_t callBackCode = 0;
        bool clickInProgress = false;
        bool imageClicked = false;
};

class GUILabel: public GUIElement
{
    public:
        GUILabel(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t screenNumber, String text, uint8_t size, uint16_t color, bool autoCenter, void (*ClickHandler)(uint8_t imageCode), uint8_t callBackCode, String** lines, uint16_t lineCount, uint16_t currPos);
        ~GUILabel() {};
        String Text = "";
        String** Lines = NULL;
        uint16_t LineCount = 0;
        uint8_t Size = 1;
        uint16_t Color = 0xFFFF;
        bool AutoCenter = false;        
        int16_t CurrPos = 0;
        void Redraw() { this->needsRedrawing = true; };
        void Run(TS_Point* clickPoint);        

    private:
        TS_Point* clickStartPoint = NULL;
        bool needsRedrawing = true;
        void (*ClickHandler)(uint8_t imageCode);
        uint8_t callBackCode = 0;
        bool clickInProgress = false;
        uint16_t MovementSpeed = 100;
        uint64_t lastMovement = 0;        
};

class GUIScroll : public GUIElement
{
    public:
        GUIScroll(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, String lines[255]);
        ~GUIScroll() { }; 
        String Lines[255]; // USE POINTER? 
        void Run(TS_Point* clickPoint);
        GUILabel* displayBlock = NULL;

    private:
        uint16_t currPos = 0;
        bool needsRedrawing = true;
        uint16_t MovementSpeed = 200;
        TS_Point* clickStartPoint = NULL;
        uint64_t lastMovement = 0;
};

class GUIGauge: public GUIElement
{
    public:
        GUIGauge(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Radius, uint16_t screenNumber, uint16_t DegreesStart, uint16_t DegreesStop, bool DrawCompleteCircle, uint16_t BackgroundColor, uint16_t ForegroundColor, uint16_t MinValue, uint16_t MaxValue, uint16_t Value, bool IsTransparent);
        ~GUIGauge() {};
        uint16_t Radius;
        uint16_t DegreesStart;
        uint16_t DegreesStop;
        bool DrawCompleteCircle;
        uint16_t BackgroundColor = 0b0000000000011111;  
        uint16_t ForegroundColor = 0b1111111111111111; 
        uint16_t MinValue = 0;
        uint16_t MaxValue = 0;
        uint16_t Value = 0;
        uint16_t MovementSpeed = 200; // ms per update
        bool IsTransparent = false;
        void Run(TS_Point* clickPoint);
        
     private:
        bool needsRedrawing = true;
        TS_Point* clickStartPoint = NULL;
        uint64_t lastMovement = 0;
};

class GUICheckBox: public GUIElement
{
    public:
      GUICheckBox(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, void (*ClickHandler)());
      void Run(TS_Point* clickPoint);        
      ~GUICheckBox(){ }

    private:  
      bool clickInProgress = false;
      void (*ClickHandler)();
      bool needsRedrawing = true;
      bool boxClickedInProgress = false;
};

//class GUINumScroll: GUIElement
//{
//  public:
//    GUINumScroll(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, uint8_t currNum);
//    ~GUINumScroll() {};
//     void Run(TS_Point* clickPoint);
//     
//  private:
//    std::ostringstream* s = new std::ostringstream;
//    uint8_t currNum = 0;
//    bool needsRedrawing = true;
//    uint64_t lastMovement = 0;
//    uint16_t MovementSpeed = 200;
//};
  
class GUI
{
    public:
        GUI();
        ~GUI();
        Adafruit_ILI9341* GetTFT() { return this->tft; };
        XPT2046_Touchscreen* GetTS() { return this->touch; };       
        void RegisterElement(GUIElement* element);
        void Run();
        uint16_t ScreenNumber = 1;

    private:        
        Adafruit_ILI9341* tft;
        XPT2046_Touchscreen* touch;
        char page = 'S';
        uint16_t vi1 = 65535, vj1 = 65535, vi2 = 65535, vj2 = 65535; // Touch Screen Calibration
        void drawScreen();
        const uint16_t CAL_MARGIN = 20;
        void calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj);
        void calibrateTouchScreen();
        void applyCalibration(TS_Point &p);
        GUIElement** elements = NULL; // Array of pointers
        uint16_t elementCount = 0;
};

class CANMessage
{
    public:
        CANMessage(uint32_t id) 
        {
            this->ID = id;
            this->Length = 0;
            this->Timestamp = millis();
            this->IsExtended = 0;
            this->IsRemoteRequest = 0;
            for(uint8_t i = 0; i < 8; i++) this->Data[i] = 0;            
        };
        uint64_t Timestamp;
        uint32_t ID;
        uint8_t Data[8];  
        uint8_t Length;
        bool IsExtended;
        bool IsRemoteRequest;

        String ToString();
};

class CAN
{
    public:
        CAN(uint8_t Pin);
        bool Init(uint8_t speed);
        bool AutoDetectSpeed(GUILabel* statusLabel);
        uint8_t GetSpeed() { return this->speed; };
        static String GetSpeedString(uint8_t speed);
        bool Transmit(CANMessage* message);
        CANMessage* Receive();        

    private:
        uint8_t pin;
        uint8_t speed;
        MCP_CAN* interface = NULL;   
        bool initialised = false;
        void setStatus(String status);
        GUILabel* statusLabel = NULL;
};

class SDCard
{
    public:
        SDCard(uint8_t pin);
        ~SDCard() {};
        bool Init();
        bool IsEnabled() { return this->enabled; };
        File CreateFile(String filename);
        File OpenFile(String filename);
        bool WriteCanMessage(File& file, CANMessage* message);

    private:
        uint8_t pin;
        bool enabled = false;
};

class SniffedCANMessage
{
    public:
        SniffedCANMessage() {};
        ~SniffedCANMessage() {};
    uint32_t ID;
    bool IsExtended;
    bool IsRemoteRequest;        
    uint8_t Length;
    
    uint64_t Interval;    
    uint8_t* Data[8];  
    uint8_t DataValueCount[8];    
};

class CANSniffer
{
    public:
        CANSniffer() {};
        ~CANSniffer() {};
        uint32_t StartingCANID = 0;
        uint32_t StoppingCanID = 4294967296;
        void SetCANReceiver(CAN* receiver) { this->receiver = receiver; };
        CAN* GetCANReceiver() { return this->receiver; };
        void SetSDCard(SDCard* sdCard) { this->sdCard = sdCard; };
        SDCard* GetSDCard() { return this->sdCard; };
        bool GetEnabled() { return this->enabled; };
        bool Start(uint32_t sessionID);
        void Stop();
        uint16_t BufferSize = 100;
        CANMessage** Buffer = NULL;
        bool BufferFull = false;

        SniffedCANMessage* GetResults() { return this->Results; };
        SniffedCANMessage* Results = NULL;
        uint32_t ResultCount = 0;
        
        void Run();

        int testcounter = 0;

    private:
        static void processor();
        static CANSniffer* activeSniffer;
        CAN* receiver = NULL;
        SDCard* sdCard = NULL;
        bool enabled = false;
        File file;
        uint32_t sessionID = 0;
        CANMessage** internalBuffer = NULL;
        uint16_t internalBufferPointer = 0;



        
//        void messageBuffer();
//        bool checkTimer(uint8_t start, uint8_t duration);
        uint8_t referenceTime = 0;
        uint16_t snifferBufferSize = 0;
        uint16_t bufferPointer = 0;
        bool bufferFull;

        
        
        
};

class CANFuzzer
{
    public:
        CANFuzzer();
        ~CANFuzzer();
        uint8_t Init();
        void Run();
        uint8_t AutoDetectCANSpeed();
        CAN* GetCANReceiver() { return this->receiver; };
        CAN* GetCANTransmitter() { return this->transmitter; };   
        SDCard* GetSDCard() { return this->sdCard; };

        GUILabel* statusLabel = NULL;        

    private:
        CAN* transmitter;
        CAN* receiver;
        SDCard* sdCard;
        void setStatus(String status);
};
