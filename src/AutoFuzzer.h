#pragma once

#include <SPI.h>
#include <SD.h>
#include "mcp_can.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"



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
        void Redraw() { this->needsRedrawing = true; };

    protected:
        void fillArc(uint16_t x, uint16_t y, uint16_t start_angle, uint16_t end_angle, uint16_t radiusX, uint16_t radiusY, uint16_t width, uint16_t colour);
        uint16_t turnToGrayScale(uint16_t color);
        bool needsRedrawing = true;
};

class GUIImage: public GUIElement
{
    public:
        GUIImage(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t screenNumber, const uint8_t* Image, uint16_t ImageLength, void (*ClickHandler)(uint8_t imageCode), uint8_t callBackCode);
        ~GUIImage() {};
        const uint8_t* Image;
        uint16_t ImageLength;
        void Run(TS_Point* clickPoint); 
        bool inClickHandler = false;      

    private:
        TS_Point* clickStartPoint = NULL;        
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
        void (*ClickHandler)(uint8_t imageCode);
        uint8_t callBackCode = 0;
        bool clickInProgress = false;
        uint16_t MovementSpeed = 100;
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
        TS_Point* clickStartPoint = NULL;
        uint64_t lastMovement = 0;
};

class GUICheckBox: public GUIElement
{
    public:
      GUICheckBox(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, void (*ClickHandler)(uint8_t boxCode), uint8_t boxCode);
      void Run(TS_Point* clickPoint);        
      ~GUICheckBox(){ }
       bool BoxClickedInProgress = false;
       bool inClickHandler = false; 
    private:  
      uint8_t boxCode = 0;
      bool clickInProgress = false;
      void (*ClickHandler)(uint8_t boxCode);
};

class GUINumScroll: public GUIElement
{
  public:
    GUINumScroll(Adafruit_ILI9341* tft, XPT2046_Touchscreen* touch, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t screenNumber, uint8_t currNum);
    ~GUINumScroll() {};
    void Run(TS_Point* clickPoint);
    uint8_t CurrNum = 0;
    bool inClickHandler = false; 
  private:
    TS_Point* clickStartPoint = NULL; 
    uint64_t lastMovement = 0;
    uint16_t MovementSpeed = 100;
    String numString = "";
};
  
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
        uint16_t GetElementsCount() { return this->elementCount; };
        GUIElement** GetElements() { return this->elements; }; 

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
        CANMessage() {};
        CANMessage(uint32_t id) { this->ID = id; };
        ~CANMessage() {};
        uint64_t Timestamp = 0;
        uint32_t ID = 0;
        uint8_t Data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };  
        uint8_t Length = 0;
        bool IsExtended = false;
        bool IsRemoteRequest = false;

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
        SDCard() {};
        ~SDCard() {};
        bool Init();
        bool IsEnabled() { return this->enabled; };
        void DeleteFile(String filename);
        File CreateFile(String filename);
        File OpenFile(String filename);
        bool WriteCanMessage(File& file, CANMessage* message);
        // bool WriteSniffedCanMessage(File& file, CANMessage* message);
        CANMessage* ReadCanMessage(File& file);
        // CANMessage* ReadSniffedCanMessage(File& file);
        bool WriteBuffer(File& file, uint8_t* buffer, uint16_t length);

    private:
        uint8_t pin;
        bool enabled = false;
};

class SniffedCANMessage
{
    public:
        SniffedCANMessage() {};
        ~SniffedCANMessage() {};
        uint32_t ID = 0;
        bool IsExtended = false;
        bool IsRemoteRequest = false;
        uint8_t Length = 0;
        
        uint64_t Interval = 0;    
        uint32_t InstanceCount = 0;
        uint8_t* Data[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };  
        uint16_t DataValueCount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        void ProcessMessage(CANMessage* message);

    private:
        uint64_t lastInterval = 0;
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

        SniffedCANMessage* GetResults() { return this->Results; };
        SniffedCANMessage* Results = NULL;
        uint32_t ResultCount = 0;        
        void Run();
        GUILabel* statusLabel = NULL;
    private:
        static void processor();
        CAN* receiver = NULL;
        SDCard* sdCard = NULL;
        bool enabled = false;
        File file;
        uint32_t sessionID = 0;
        uint64_t timeStarted = 0;
        void setStatus(String status);       
};

enum CANFuzzerModes { Analyse, None, Manual, Automatic, Unique};
enum CANFuzzerInputs { SnifferFile, LiveCapture };
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
        bool GetEnabled() { return this->enabled; };       
        bool Start(uint32_t sessionID, CANFuzzerModes mode, CANFuzzerInputs input);
        void Stop();
        uint32_t FuzzedID = 0;
        uint8_t FuzzedBytes = 0;
        
        GUILabel* statusLabel = NULL;
        String** idStrings();
        String** ids;
        uint32_t count = 0;
        uint32_t pos = 0;
               

    private:
        CAN* transmitter;
        CAN* receiver;
        SDCard* sdCard;
        CANFuzzerModes mode = None;
        CANFuzzerInputs input = LiveCapture;
        File inputFile;
        File analysedFile;
        uint32_t sessionID = 0;
        uint64_t timeStarted = 0;
        bool enabled = false;
        uint8_t percentComplete = 0;
        void setStatus(String status);
        CANMessage* getNextMessage();
        SniffedCANMessage** sniffedMessages = NULL;
        uint16_t sniffedMessagesCount = 0;
       
};
