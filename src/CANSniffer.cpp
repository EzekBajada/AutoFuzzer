#include "AutoFuzzer.h"

bool CANSniffer::Start(uint32_t sessionID)
{
    if (this->enabled) return false;
    this->sessionID = sessionID;
    this->file = this->sdCard->CreateFile(String(F("/AutoFuzzer/Sniffer/")) + String(this->sessionID) + String(F(".Sniffed")));
    if (!this->file) Serial.println("ERROR");
    if (this->file) this->enabled = true; else this->enabled = false;
    return this->enabled;
}

void CANSniffer::Stop()
{
    if (this->file) this->file.close();
    this->sessionID = 0;
    this->enabled = false;
}

void CANSniffer::Run()
{
    if (this->enabled)
    {
        CANMessage* message = this->receiver->Receive();
        if (message)
        {
            if (!this->sdCard->WriteCanMessage(this->file, message))
            {
                Serial.println("WRITE FAILED!!!"); // DEBUG
            }
            if (message->ID == 0x0AA)
            {
                Serial.println(millis());
            }
            delete message;
        }
    }  
}
