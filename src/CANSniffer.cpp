#include "AutoFuzzer.h"

bool CANSniffer::Start(uint32_t sessionID)
{
    if (this->enabled) return false;
    this->sessionID = sessionID;   
    this->sdCard->DeleteFile(String(F("S")) + String(this->sessionID));
    this->file = this->sdCard->CreateFile(String(F("S")) + String(this->sessionID));
    if (!this->file) this->setStatus(F("Error creating Sniffed File..."));
    if (this->file) this->enabled = true; else this->enabled = false;
    if (this->enabled) this->timeStarted = millis();
    if (this->enabled) this->setStatus(F("Sniffing..."));
    return this->enabled;
}

void CANSniffer::setStatus(String status)
{
    if (this->statusLabel == NULL) return;
    this->statusLabel->Text = status;
    this->statusLabel->Redraw();
    this->statusLabel->Run(NULL);
}

void CANSniffer::Stop()
{
    if (this->file) this->file.close();
    this->sessionID = 0;
    this->enabled = false;
    this->setStatus(F("Sniffing Ended"));
}

void CANSniffer::Run()
{    
    if (this->enabled)
    {
        CANMessage* message = this->receiver->Receive();
        if (message)
        {
            message->Timestamp = millis() - this->timeStarted;
            if (!this->sdCard->WriteCanMessage(this->file, message)) Serial.println("SD Write Failed!");            
            delete message;
        }        
    }
}
