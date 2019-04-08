#include "AutoFuzzer.h"

CAN::CAN(uint8_t Pin)
{
    this->initialised = false;
    this->Pin = Pin;
    
    this->interface = new MCP_CAN(this->Pin);    
}

bool CAN::Init(uint8_t Speed)
{
    this->initialised = false;
    uint64_t timeout = millis() + 1000;
    while (CAN_OK != this->interface->begin(Speed)) 
    {
        delay(100);
        if (timeout < millis()) { this->Speed = 0; return false; }
    }
    this->Speed = Speed;
    this->initialised = true;
    return true;
}

bool CAN::Init() // AutoDetect CANBus Speed
{
    this->Speed = 0;
    return false; 
}

bool CAN::Transmit(CANMessage* message)
{
    if (!this->initialised) return false;
    return this->interface->sendMsgBuf(message->id, message->isExtended ? 1:0, message->length, message->data) == CAN_OK; 
}
