#include "AutoFuzzer.h"

CANFuzzer::CANFuzzer()
{
    this->transmitter = new CAN(D4);
    this->receiver = new CAN(D3);
}

CANFuzzer::~CANFuzzer()
{
    delete this->transmitter;
    delete this->receiver;
}

bool CANFuzzer::Init()
{
    return false;
}

void CANFuzzer::Run()
{
    
}
