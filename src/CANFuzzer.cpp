#include "AutoFuzzer.h"

CANFuzzer::CANFuzzer()
{
    this->transmitter = new CAN(D4);
    this->receiver = new CAN(D3);
    this->sdCard = new SDCard(D1);
}

CANFuzzer::~CANFuzzer()
{
    delete this->transmitter;
    delete this->receiver;
}

void CANFuzzer::setStatus(String status)
{
    if (this->statusLabel == NULL) return;
    this->statusLabel->Text = status;
    this->statusLabel->Redraw();
    this->statusLabel->Run(NULL);
}

uint8_t CANFuzzer::Init()
{   
    this->setStatus(F("Initialising..."));
    if (!this->receiver->Init(CAN_100KBPS)) 
    {
        this->setStatus(F("Error Initialising CAN RX!"));
        return 1;
    }
    if (!this->transmitter->Init(CAN_100KBPS))
    {
        this->setStatus(F("Error Initialising CAN TX!"));
        return 2;
    }
    if (!this->sdCard->Init())
    {
        this->setStatus(F("Error Initialising SDCard!"));
        return 3; 
    }    
    this->setStatus(F("Initialisation successful"));
    return 0;
}

bool CANFuzzer::Start(uint32_t sessionID, CANFuzzerModes mode, CANFuzzerInputs input)
{
    if (this->enabled) return false;
    this->sessionID = sessionID; 
    this->mode = mode;
    this->input = input;
    switch(this->mode)
    {
        case Analyse:
        {
            this->setStatus(F("Starting Analysis..."));
            this->enabled = true;
            switch(this->input)
            {
                case SnifferFile:
                {
                    this->inputFile = this->sdCard->OpenFile(String(F("S")) + String(this->sessionID));
                    if (!this->inputFile)
                    {
                        this->setStatus(F("Sniffed File not Found!"));
                        this->enabled = false;
                    }                  
                }
                break;
                case LiveCapture:
                {
                }
                break;
            }
            if (this->enabled)
            {             
                this->sdCard->DeleteFile(String(F("A")) + String(this->sessionID));
                this->analysedFile = this->sdCard->CreateFile(String(F("A")) + String(this->sessionID));
                if (!this->analysedFile)
                {
                    this->setStatus(F("Analysed File Creation Failed!"));
                    this->enabled = false;
                }
            }
        }
        break;
        case None:
        {
           
        }
        break;
        case Manual:
        {
           
        }
        break;
        case Automatic:
        {
           
        }
        break;
    }      
    if (this->enabled)
    {
        this->timeStarted = millis();
        this->percentComplete = 0;
    }
    return this->enabled;
}

void CANFuzzer::Stop()
{
    if (this->inputFile) this->inputFile.close();
    if (this->analysedFile) this->analysedFile.close();
    this->sessionID = 0;
    this->enabled = false;    
}

uint8_t CANFuzzer::AutoDetectCANSpeed()
{
    this->setStatus(F("AutoDetecting CAN Speed..."));
    if (!this->receiver->AutoDetectSpeed(this->statusLabel))
    {
        this->setStatus(F("Error AutoDetecting CAN Speed!"));
        return 1; // Error AutoDetecting RX Speed
    }
    if (!this->transmitter->Init(this->receiver->GetSpeed()))
    {
        this->setStatus(F("Error Setting TX CAN Speed!"));
        return 2; // Error Setting TX Speed to follow RX
    }
    this->setStatus(String(F("CAN Speed Detected at ")) + CAN::GetSpeedString(this->receiver->GetSpeed()) + String(F("kbps")));
}

CANMessage* CANFuzzer::getNextMessage()
{
    switch(this->input)
    {
        case SnifferFile: return this->sdCard->ReadCanMessage(this->inputFile);
        case LiveCapture: this->receiver->Receive();
        default: return NULL;        
    }
}

void CANFuzzer::Run()
{
    if (!this->enabled) return;
    switch(this->mode)
    {
        case Analyse:
        {
            if (this->input == SnifferFile && this->inputFile.position() + 10 > this->inputFile.size())
            { // File Processed
                this->setStatus(F("Analysis Complete."));
                this->Stop();              
            }
            CANMessage* message = this->getNextMessage();
            if (message != NULL)
            {
                Serial.println(message->ToString());
                
                // analysis goes here
                
                delete message;
            }
        }
        break;
        case None:
        {
           
        }
        break;
        case Manual:
        {
           
        }
        break;
        case Automatic:
        {
           
        }
        break;
    }    
}
