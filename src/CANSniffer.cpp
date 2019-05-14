#include "AutoFuzzer.h"

CANSniffer* CANSniffer::activeSniffer = NULL;

void ICACHE_RAM_ATTR CANSniffer::processor()
{
    if (CANSniffer::activeSniffer == NULL || CANSniffer::activeSniffer->internalBuffer == NULL || CANSniffer::activeSniffer->Buffer == NULL) return;  
    CANMessage* message = CANSniffer::activeSniffer->receiver->Receive();
    if (message)
    {        
        if (CANSniffer::activeSniffer->internalBufferPointer + 1 >= CANSniffer::activeSniffer->BufferSize)
        {
            for(uint16_t i = 0; i < CANSniffer::activeSniffer->BufferSize; i++)      
            {
                if (CANSniffer::activeSniffer->Buffer[i]) 
                {
                    delete CANSniffer::activeSniffer->Buffer[i];
                    CANSniffer::activeSniffer->Buffer[i] = NULL;
                }
            }
            if(memcpy(CANSniffer::activeSniffer->Buffer, CANSniffer::activeSniffer->internalBuffer, sizeof(CANMessage*) * CANSniffer::activeSniffer->BufferSize))
            {
                //Serial.println("Buffer Copied");
                CANSniffer::activeSniffer->BufferFull = true;
            }
            else
            {
                Serial.println("Error copying buffer!");
            }
            CANSniffer::activeSniffer->internalBufferPointer = 0;
        }
        CANSniffer::activeSniffer->internalBuffer[CANSniffer::activeSniffer->internalBufferPointer++] = message;        
    }    
    timer0_write(ESP.getCycleCount() + 160000);  // 1ms   
}

bool CANSniffer::Start(uint32_t sessionID)
{
    if (this->enabled) return false;
    this->sessionID = sessionID;
   // this->file = this->sdCard->CreateFile(String(F("AutoFuzzer/Sniffer/")) + String(this->sessionID) + String(F(".Sniffed")));
    this->file = this->sdCard->CreateFile(String(F("Sniffed"))+String(this->sessionID));
    if (!this->file) Serial.println("ERROR");
    if (this->file) this->enabled = true; else this->enabled = false;/*
    if (this->enabled)
    {
        this->internalBuffer = (CANMessage**) malloc(sizeof(CANMessage*) * this->BufferSize);
        this->Buffer = (CANMessage**) malloc(sizeof(CANMessage*) * this->BufferSize);
        for(uint16_t i = 0; i < this->BufferSize; i++)this->Buffer[i] = NULL;
        this->BufferFull = false;
        CANSniffer::activeSniffer = this;
        this->internalBufferPointer = 0;
        noInterrupts();
        timer0_isr_init();
        timer0_attachInterrupt(processor);
        timer0_write(ESP.getCycleCount() + 160000);  // 1ms   
        interrupts(); 
    }*/
    return this->enabled;
}

void CANSniffer::Stop()
{
//    timer0_detachInterrupt();
    if (this->file) this->file.close();
    this->sessionID = 0;
    CANSniffer::activeSniffer = NULL;   
/*    this->BufferFull = false; 
    
    if (this->internalBuffer)
    {
        for(uint16_t i = 0; i < this->BufferSize; i++)
        {
            if (this->internalBuffer[i]) delete this->internalBuffer[i];      
        }
        free(this->internalBuffer);        
    }
    this->internalBuffer = NULL;      
    
    if (this->Buffer)
    {
       free(this->Buffer);
    }
    this->Buffer = NULL;*/
    this->enabled = false;    
}

static long lastTime = 0;

void CANSniffer::Run()
{  
    
    if (this->enabled)
    {
        CANMessage* message = this->receiver->Receive();
        if (message)
        {
            
            if (message->ID == 0x0AA) 
            {
                long now = millis();
                Serial.println(now - lastTime);
                lastTime = now;
            }
            
            if (!this->sdCard->WriteCanMessage(this->file, message)) Serial.println("WRITE FAILED!!!"); // DEBUG
            delete message;
        }
        
    }
    

  /*
    if (this->enabled)
    {
        if (this->Buffer != NULL && this->BufferFull)
        {
            for(uint16_t i = 0; i < this->BufferSize; i++)
            {
                CANMessage* message = this->Buffer[i];
                if (message)
                {               
                    if (!this->sdCard->WriteCanMessage(this->file, message))
                    {
                        Serial.println("WRITE FAILED!!!"); // DEBUG
                    }
                    delete message;
                    this->Buffer[i] = NULL;
                }
            }
            this->BufferFull = false;

        }  
    }*/
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
