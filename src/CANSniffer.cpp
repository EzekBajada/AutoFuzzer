#include "AutoFuzzer.h"

CANSniffer* CANSniffer::activeSniffer = NULL;

bool CANSniffer::Start()
{
    if (this->enabled) return false;
    this->file = this->sdCard->CreateFile(String(F("Sniffed"))+this->sessionID);
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
    this->sessionID = "";
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
