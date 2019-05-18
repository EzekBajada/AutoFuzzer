#include "AutoFuzzer.h"

SDCard::SDCard(uint8_t pin)
{
    this->pin = pin;    
}

bool SDCard::Init()
{
    this->enabled = SD.begin(this->pin); 
    if(this->enabled)
    { 
        SD.mkdir("/AutoFuzzer");   // NOT WORKING!!!
        if (!SD.exists(F("/AutoFuzzer/Sniffer"))) SD.mkdir(F("/AutoFuzzer/Sniffer"));
        if (!SD.exists(F("/AutoFuzzer/Fuzzer"))) SD.mkdir(F("/AutoFuzzer/Fuzzer"));
    }
    return this->enabled;
}

File SDCard::CreateFile(String filename)
{    
    SD.remove(filename);
    return SD.open(filename, FILE_WRITE);
}

File SDCard::OpenFile(String filename)
{
    return SD.open(filename, FILE_READ);
}

bool SDCard::WriteCanMessage(File& file, CANMessage* message)
{
    if(!file) return false;     
    SPI.setFrequency(20000000);
    return file.write((uint8_t*) &message, sizeof(CANMessage)) == sizeof(CANMessage);
}

bool SDCard::WriteBuffer(File& file, uint8_t* buffer, uint16_t length)
{
    if(!file) return false;
    return file.write(buffer, length) == length;
}
