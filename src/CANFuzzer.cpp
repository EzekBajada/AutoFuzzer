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

void CANFuzzer::Analyse(File file, uint32_t sessionID)
{
    this->sessionID = sessionID;
    file = this->sdCard->CreateFile(String(F("Analysed"))+String(this->sessionID));
    if (!file) Serial.println("ERROR");
    if (file) this->enabled = true; else this->enabled = false;
}
void CANFuzzer::Run()
{
    
}
