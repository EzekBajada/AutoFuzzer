#include "AutoFuzzer.h"

CANFuzzer::CANFuzzer()
{
    this->transmitter = new CAN(D4);
    this->receiver = new CAN(D3);
    this->sdCard = new SDCard(D1);
    srand(micros());
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
            if (this->enabled)
            {
                this->setStatus(F("Analysis in Progress...")); 
                if (this->sniffedMessagesCount > 0)
                {
                    for(uint16_t i = 0; i < this->sniffedMessagesCount; i++) delete this->sniffedMessages[i];
                    free(this->sniffedMessages);
                    this->sniffedMessagesCount = 0;
                    this->sniffedMessages = NULL;
                }
            }
        }
        break;
        case None: // Playback
        {
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
            this->setStatus(F("Playback in Progress..."));
        }
        break;
        case Manual:
        {
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
            this->setStatus(F("Manual Fuzzing in Progress..."));          
        }
        break;
        case Automatic:
        {
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

             // reading of sniffed message to file goes here
            this->setStatus(F("Auto Fuzzing in Progress..."));                    
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
    this->setStatus(F("Fuzzing Ended."));
    // clearing of sniffed messages goes here
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
        case SnifferFile: 
        {
            CANMessage* message = this->sdCard->ReadCanMessage(this->inputFile);
          
            if (message == NULL) 
            {
              return NULL;
              }
            if (this->mode != Analyse)               
                while(message->Timestamp > millis() - this->timeStarted) yield();            
            return message;
        }
        case LiveCapture: return this->receiver->Receive();
        default: return NULL;        
    }
}
String** CANFuzzer::idStrings()
{
     this->inputFile = this->sdCard->OpenFile(String(F("S")) + String(this->sessionID));
     if (!this->inputFile)
     {
        this->setStatus(F("Sniffed File not Found!"));
        this->enabled = false;
     }
     
     for(int i=0;i<this->inputFile.size();i++)
     {
        
        CANMessage* message = this->getNextMessage();
        
        if(message != NULL)
        {
          Serial.println("POS:");
          this->ids[pos] = NULL;
//          this->ids[pos] = 33;    
         if(this->ids[pos] == NULL) Serial.println("its null");
          
          
          
          if(this->ids[pos] == NULL)
        {
          Serial.println("pos");
          String currId = String(message->ID);
          ids[0][pos] = currId;
          this->pos++;
          this->count++;
        }
        }
        
      }   
     
    
     return this->ids;
}
uint32_t** CANFuzzer::canIds(uint32_t sessionID)
{
  this->inputFile = SD.open("S1", FILE_READ);
  Serial.println(this->inputFile.size());
  if (!this->inputFile)
  {
      this->setStatus(F("Sniffed File not Found!"));
      this->enabled = false;
  }
  this->input = SnifferFile;
  CANMessage* message = this->getNextMessage();
  if(message != NULL) Serial.print(message->ID);
  return this->cans;
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
                this->Stop(); 
            }
            CANMessage* message = this->getNextMessage();
            if (message != NULL)
            {                                
                // analysis goes here 
                SniffedCANMessage* sniffedMessage = NULL;
                for(uint16_t i = 0; i < this->sniffedMessagesCount && sniffedMessage == NULL; i++)
                    if (this->sniffedMessages[i]->ID == message->ID && this->sniffedMessages[i]->Length == message->Length) sniffedMessage = this->sniffedMessages[i];
                if (!sniffedMessage)
                { 
                    sniffedMessage = new SniffedCANMessage();
                    sniffedMessage->ID = message->ID;
                    sniffedMessage->Length = message->Length;
                    ++this->sniffedMessagesCount;
                    if (this->sniffedMessagesCount == 1)
                        this->sniffedMessages = (SniffedCANMessage**) malloc(sizeof(SniffedCANMessage*));
                    else
                        this->sniffedMessages = (SniffedCANMessage**) realloc(this->sniffedMessages, sizeof(SniffedCANMessage*) * this->sniffedMessagesCount);
                    this->sniffedMessages[this->sniffedMessagesCount - 1] = sniffedMessage;
                }
                sniffedMessage->ProcessMessage(message);
                // writing of sniffed message to file goes here               

                
                Serial.println(message->ToString() + String(ESP.getFreeHeap()));
                delete message;
            }
        }
        break;
        case None:
        {
            if (this->input == SnifferFile && this->inputFile.position() + 10 > this->inputFile.size())
            { // File Processed                
                this->Stop();              
            }
            CANMessage* message = this->getNextMessage();
            if (message != NULL)
            {
                if(message->ID == 304)
                {
                    Serial.println("BUTTON ID: ");
                    Serial.println(message->ID);
                }
                this->transmitter->Transmit(message);            
                delete message;
            }           
        }
        break;
        case Manual:
        {
            if (this->input == SnifferFile && this->inputFile.position() + 10 > this->inputFile.size())
            { // File Processed                
                this->Stop();              
            }
            CANMessage* message = this->getNextMessage();
            if (message != NULL)
            {
                if (message->ID == this->FuzzedID)                
                    for(uint8_t i = 0; i < 8; i++)
                    {
                        if ((this->FuzzedBytes & (0b10000000 >> i)) != 0)
                        {
                            message->Data[i] = (uint8_t) (rand() % 256);
                        }
                    }
                this->transmitter->Transmit(message);            
                delete message;
            }             
        }
        break;
        case Automatic: // Uses Analysed File
        {          
            
            if (this->input == SnifferFile && this->inputFile.position() + 10 > this->inputFile.size())
            { // File Processed                
                this->Stop();              
            }
            CANMessage* message = this->getNextMessage();
            if (message != NULL)
            {
                if (message->ID == this->FuzzedID)
                {
                    SniffedCANMessage* sniffedMessage = NULL;
                    for(uint16_t i = 0; i < this->sniffedMessagesCount && sniffedMessage == NULL; i++)
                        if (this->sniffedMessages[i]->ID == message->ID && this->sniffedMessages[i]->Length == message->Length) sniffedMessage = this->sniffedMessages[i];
                    if (sniffedMessage != NULL)
                    {
                        for(uint8_t i = 0; i < 8; i++)
                            if ((this->FuzzedBytes & (0b10000000 >> i)) != 0)
                            {
                                uint16_t valueIndex = (uint16_t) (rand() % sniffedMessage->DataValueCount[i]);
                                message->Data[i] = sniffedMessage->Data[i][valueIndex];
                            }
                    }
                }
                this->transmitter->Transmit(message);            
                delete message;
            }              
        }
        break;
        ;
        }
        
    }    
