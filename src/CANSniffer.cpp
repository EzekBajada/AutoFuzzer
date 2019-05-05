#include "AutoFuzzer.h"

CANSniffer* CANSniffer::activeSniffer = NULL;

void ICACHE_RAM_ATTR CANSniffer::processor()
{
    if (CANSniffer::activeSniffer == NULL || CANSniffer::activeSniffer->internalBuffer == NULL || CANSniffer::activeSniffer->Buffer == NULL) return;  
    CANMessage* message = CANSniffer::activeSniffer->receiver->Receive();
    if (message)
    {
        ++CANSniffer::activeSniffer->internalBufferPointer;
        if (CANSniffer::activeSniffer->internalBufferPointer >= CANSniffer::activeSniffer->BufferSize)
        {
            if (CANSniffer::activeSniffer->BufferFull)
            { 
                Serial.println("Buffer Overrun!");
                for(uint16_t i = 0; i < CANSniffer::activeSniffer->BufferSize; i++)        
                    if (CANSniffer::activeSniffer->internalBuffer[i] != NULL) delete CANSniffer::activeSniffer->internalBuffer[i];
            }
            else
            {
                memcpy(CANSniffer::activeSniffer->Buffer, CANSniffer::activeSniffer->internalBuffer, sizeof(CANMessage*) * CANSniffer::activeSniffer->BufferSize);
                CANSniffer::activeSniffer->BufferFull = true;
            }
            CANSniffer::activeSniffer->internalBufferPointer = 0;
        }
        CANSniffer::activeSniffer->internalBuffer[CANSniffer::activeSniffer->internalBufferPointer] = message;
    }    
    timer0_write(ESP.getCycleCount() + 160000);  // 1ms   
}

bool CANSniffer::Start(uint32_t sessionID)
{
    if (this->enabled) return false;
    this->sessionID = sessionID;
   // this->file = this->sdCard->CreateFile(String(F("AutoFuzzer/Sniffer/")) + String(this->sessionID) + String(F(".Sniffed")));
    this->file = this->sdCard->CreateFile(String(F("myfile")));
    if (!this->file) Serial.println("ERROR");
    if (this->file) this->enabled = true; else this->enabled = false;
    if (this->enabled)
    {
        this->internalBuffer = (CANMessage**) malloc(sizeof(CANMessage*) * this->BufferSize);
        this->Buffer = (CANMessage**) malloc(sizeof(CANMessage*) * this->BufferSize);
        this->BufferFull = false;
        CANSniffer::activeSniffer = this;
        this->internalBufferPointer = 0;
        noInterrupts();
        timer0_isr_init();
        timer0_attachInterrupt(processor);
        timer0_write(ESP.getCycleCount() + 160000);  // 1ms   
        interrupts(); 
    }
    return this->enabled;
}

void CANSniffer::Stop()
{
    timer0_detachInterrupt();
    if (this->file) this->file.close();
    this->sessionID = 0;
    CANSniffer::activeSniffer = NULL;   
    this->BufferFull = false; 
    if (this->internalBuffer)
    {
        for(uint16_t i = 0; i < this->BufferSize; i++)        
            if (this->internalBuffer[i] != NULL) delete this->internalBuffer[i];            
        free(this->internalBuffer);
    }
    this->internalBuffer = NULL;
    if (this->Buffer)
    {
        for(uint16_t i = 0; i < this->BufferSize; i++)        
            if (this->Buffer[i] != NULL) delete this->Buffer[i];            
        free(this->Buffer);
    }
    this->Buffer = NULL;
    this->enabled = false;
}

void CANSniffer::Run()
{
    if (this->enabled)
    {
        if (this->Buffer != NULL && this->BufferFull)
        {
            for(uint16_t i = 0; i < this->BufferSize; i++)
            {
                CANMessage* message = this->Buffer[i];
                if (message)
                {
                
                    Serial.println(message->ID);
        
                    delete message;
                    this->Buffer[i] = NULL;
                }
            }
            this->BufferFull = false;
        }
      /*
        CANMessage* message = this->receiver->Receive();
        if (message)
        {
            
            if (!this->sdCard->WriteCanMessage(this->file, message))
            {
                Serial.println("WRITE FAILED!!!"); // DEBUG
            }
            if (message->ID == 0x0AA)
            {
                testcounter++;
            }
            this->bufferPointer++;
            delete message; // To move and be called when buffer is full not with every message
        }*/
    }  
}
/*
void CANSniffer::messageBuffer(CANMessage* message)
{
  this->internalBuffer[this->bufferPointer] = message;
}*/

/*
boolean CANSniffer::checkTimer(uint8_t start, uint8_t duration, uint8_t referenceTime){
  unsigned long time = millis()/1000; 
  unsigned long startTime = referenceTime + start;
  unsigned long endTime = startTime + duration;
  if(time >= startTime && time <= endTime) return true;
}*/
