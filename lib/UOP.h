#ifndef __UOP__
#define __UOP__
#include "UOP_Adapter.h"
#include "UOP_Codec.cpp"
#include "UOP_CommonTypes.h"
#include "UOP_Interaction.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <chrono>
#include <stdio.h>

using namespace std;
typedef chrono::high_resolution_clock Time;
typedef chrono::milliseconds ms;
chrono::high_resolution_clock::time_point InitialTime;

#define byte unsigned char
#define UOP_AdaptersList_Max_Size 50


long long GetTime_ms()
{
    auto timeNow = Time::now();
    return chrono::duration_cast<ms>(timeNow - InitialTime).count();
}
void delay_ms(int delay){
    long long tn = GetTime_ms();
    while (GetTime_ms()-tn < delay);
}

class UOP
{
private:
    static const string version ;
    vector<UOP_Adapter*> AdaptersVector;
    vector<UOP_Adapter*>::iterator GetAdapterIterator(UOP_Adapter *adapter);
    UOP_Codec codec;
public:
    UOP();
    ~UOP();
    bool AddAdapter(UOP_Adapter *adapter);
    UOP_Adapter* AddNewAdapter();  
    bool RemoveAdapter(UOP_Adapter *adapter);
    void Core();


    string GetVersion();

private:

    int GetAdapterIndex(UOP_Adapter *adapter);

};
const string UOP::version = "3.0";
#endif