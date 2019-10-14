#ifndef __UOP__
#define __UOP__
#include "BufferedAdapter.h"
#include "UOP_Codec.cpp"
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
    vector<BufferedAdapter*> AdaptersVector;
    vector<BufferedAdapter*>::iterator GetAdapterIterator(BufferedAdapter *adapter);
    UOP_Codec codec;
public:
    UOP();
    ~UOP();

    bool AddAdapter(BufferedAdapter *adapter);
    BufferedAdapter* AddNewAdapter();  
    bool RemoveAdapter(BufferedAdapter *adapter);
    string GetVersion();

private:

    int GetAdapterIndex(BufferedAdapter *adapter);

};
const string UOP::version = "3.0";
#endif