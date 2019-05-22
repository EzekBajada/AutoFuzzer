#include "AutoFuzzer.h"

SDCard::SDCard(uint8_t pin)
{
    this->pin = pin;    
}

bool SDCard::Init()
{
    this->enabled = SD.begin(this->pin); 
    return this->enabled;
}

void SDCard::DeleteFile(String filename)
{
    SD.remove(filename);
}

File SDCard::CreateFile(String filename)
{
    return SD.open(filename, FILE_WRITE);
}

File SDCard::OpenFile(String filename)
{
    return SD.open(filename, FILE_READ);
}

CANMessage* SDCard::ReadCanMessage(File& file)
{
    if(!file) return false;     
    SPI.setFrequency(20000000);
    uint8_t data[14 + 8];
    CANMessage* message = new CANMessage();
    if (file.readBytes((char*) data, 13) != 13) { delete message; return NULL; }
    if (file.readBytes((char*) (data + 13), data[12] + 1) != data[12] + 1) { delete message; return NULL; }
    uint8_t pos = 0;
    message->Timestamp = 0;
    for(uint8_t i = 0; i < 8; i++) message->Timestamp |= (data[pos++] << (i * 8));
    message->ID = 0;
    for(uint8_t i = 0; i < 4; i++) message->ID |= (data[pos++] << (i * 8));
    message->Length = data[pos++];
    for(uint8_t i = 0; i < message->Length; i++) message->Data[i] = data[pos++];
    message->IsExtended = ((data[pos] &= 0b00000001) != 0);
    message->IsRemoteRequest = ((data[pos] &= 0b00000010) != 0);
    return message;
}
//bool SDCard::WriteSniffedCanMessage(File& file, SniffedCANMessage* message)
//{
//  if(!file) return false;
//  SPI.setFrequency(20000000);
//  uint8_t data[14 + message->Length];
//    uint16_t pos = 0;    
//    for(uint8_t i = 0; i < 8; i++) data[pos++] = (uint8_t) ((message->Timestamp >> (i * 8)) & 0xFF);
//    for(uint8_t i = 0; i < 4; i++) data[pos++] = (uint8_t) ((message->ID >> (i * 8)) & 0xFF);
//    data[pos++] = message->Length;    
//    for(uint8_t i = 0; i < message->Length; i++) data[pos++] = message->Data[i];
//}
bool SDCard::WriteCanMessage(File& file, CANMessage* message)
{
    if(!file) return false;     
    SPI.setFrequency(20000000);
    uint8_t data[14 + message->Length];
    uint16_t pos = 0;    
    for(uint8_t i = 0; i < 8; i++) data[pos++] = (uint8_t) ((message->Timestamp >> (i * 8)) & 0xFF);
    for(uint8_t i = 0; i < 4; i++) data[pos++] = (uint8_t) ((message->ID >> (i * 8)) & 0xFF);
    data[pos++] = message->Length;
    for(uint8_t i = 0; i < message->Length; i++) data[pos++] = message->Data[i];
    data[pos] = 0;
    if (message->IsExtended) data[pos] |= 0b00000001;
    if (message->IsRemoteRequest) data[pos] |= 0b00000010;
    ++pos;
    return file.write((uint8_t*) &data, 14 + message->Length) == 14 + message->Length;    
}

bool SDCard::WriteBuffer(File& file, uint8_t* buffer, uint16_t length)
{
    if(!file) return false;
    return file.write(buffer, length) == length;
}
