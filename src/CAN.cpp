#include "AutoFuzzer.h"

void SniffedCANMessage::ProcessMessage(CANMessage* message)
{
    if (message->ID != this->ID || message->Length != this->Length) return;
    this->IsExtended |= message->IsExtended;
    this->IsRemoteRequest |= message->IsRemoteRequest;
    uint64_t interval = this->lastInterval - message->Timestamp;
    this->Interval *= this->InstanceCount;
    this->Interval += interval;
    this->InstanceCount++;
    this->Interval /= this->InstanceCount;;
    this->lastInterval = message->Timestamp;
    for(uint8_t i = 0; i < 8; i++)
    {
        bool exists = false;
        for(uint16_t j = 0; j < this->DataValueCount[i] && !exists; j++) if (this->Data[i][j] == message->Data[i]) exists = true;
        if (!exists)
        {
            ++this->DataValueCount[i];
            if (this->DataValueCount[i] == 1)
                this->Data[i] = (uint8_t*) malloc(1);
            else
                this->Data[i] = (uint8_t*) realloc(this->Data[i], this->DataValueCount[i]);
            this->Data[i][this->DataValueCount[i] - 1] = message->Data[i];
        }
    }
}
String CANMessage::ToString()
{
    String s = String(this->ID, HEX);
    s.toUpperCase();
    while (s.length() < 4) s = String(F(" ")) + s;
    s += String(F(" - "));
    for(uint8_t i = 0; i < this->Length; i++) 
    {
        String num = String(this->Data[i], HEX);
        num.toUpperCase();
        while (num.length() < 2) num = String(F(" ")) + num;
        s += num + String(" ");
    }
    if (this->IsExtended) s += F(" E");
    if (this->IsRemoteRequest) s += F(" R");
    return s;
}

CAN::CAN(uint8_t Pin)
{
    this->initialised = false;
    this->pin = Pin;
    this->interface = new MCP_CAN(this->pin);    
}

bool CAN::Init(uint8_t speed)
{
    this->initialised = false;
    uint64_t timeout = millis() + 1000;
    while (CAN_OK != this->interface->begin(speed)) 
    {
        delay(100);
        if (timeout < millis()) { this->speed = 0; return false; }
    }
    this->speed = speed;
    this->initialised = true;
    return true;
}

String CAN::GetSpeedString(uint8_t speed)
{
    switch(speed)
    {
        case CAN_5KBPS: return F("5");
        case CAN_10KBPS: return F("10");
        case CAN_20KBPS: return F("20");
        case CAN_25KBPS: return F("25");
        case CAN_31K25BPS: return F("31.25");
        case CAN_33KBPS: return F("33");
        case CAN_40KBPS: return F("40");
        case CAN_50KBPS: return F("50");
        case CAN_80KBPS: return F("80");
        case CAN_83K3BPS: return F("83.3");
        case CAN_95KBPS: return F("95");
        case CAN_100KBPS: return F("100");
        case CAN_125KBPS: return F("125");
        case CAN_200KBPS: return F("200");
        case CAN_250KBPS: return F("250");
        case CAN_500KBPS: return F("500");
        case CAN_666KBPS: return F("666");
        case CAN_1000KBPS: return F("1000");
        default: return F("Unknown");
    }
}

void CAN::setStatus(String status)
{
    if (this->statusLabel == NULL) return;
    this->statusLabel->Text = status;
    this->statusLabel->Redraw();
    this->statusLabel->Run(NULL);
}

bool CAN::AutoDetectSpeed(GUILabel* statusLabel) // AutoDetect CANBus Speed
{
    this->statusLabel = statusLabel;
    uint8_t speeds[] = {CAN_5KBPS, CAN_10KBPS, CAN_20KBPS, CAN_25KBPS, CAN_31K25BPS,CAN_33KBPS, CAN_40KBPS, CAN_50KBPS, CAN_80KBPS, CAN_83K3BPS, CAN_95KBPS, CAN_100KBPS, CAN_125KBPS, CAN_200KBPS, CAN_250KBPS, CAN_500KBPS, CAN_666KBPS, CAN_1000KBPS};
    for(uint8_t i = 11; i < 18; i++)
    {
        this->setStatus(String(F("Checking ")) + CAN::GetSpeedString(speeds[i]) + String(F("kbps")));
        //Serial.print(String(F("Checking ")) + CAN::GetSpeedString(speeds[i]) + String(F("kbps...")));
        if (this->Init(speeds[i]))
        {
            //Serial.print("Init OK... Awaiting Messages...");
            uint64_t timeout = millis() + 1000;
            while (timeout > millis()) 
            {
                CANMessage* message = this->Receive();
                if (message != NULL)
                {
                    //Serial.println("OK.");
                    return true;                  
                }
                if(this->statusLabel != NULL) this->setStatus(this->statusLabel->Text + String(F(".")));
                delay(100);              
            }
            //Serial.println("No messages received."); 
        }
        //else Serial.println("Failed to set Speed.");        
    }
    return false;
}

bool CAN::Transmit(CANMessage* message)
{
    if (!this->initialised) return false;
    return this->interface->sendMsgBuf(message->ID, message->IsExtended ? 1:0, message->Length, message->Data) == CAN_OK; 
}

CANMessage* CAN::Receive()
{
    if (CAN_MSGAVAIL != this->interface->checkReceive()) return NULL;
    CANMessage* message = new CANMessage();
    this->interface->readMsgBuf(&message->Length, message->Data);
    message->ID = this->interface->getCanId();   
    return message;
}
