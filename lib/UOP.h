#ifndef __UOP__
#define __UOP__
#include "BufferedAdapter.h"
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>

using namespace std;
typedef chrono::high_resolution_clock Time;
typedef chrono::milliseconds ms;
chrono::high_resolution_clock::time_point InitialTime;

#define byte unsigned char

long long GetTime_ms()
{
    auto timeNow = Time::now();
    return chrono::duration_cast<ms>(timeNow - InitialTime).count();
}
void delay_ms(int delay){
    long long tn = GetTime_ms();
    while (GetTime_ms()-tn < delay);
}
typedef enum UOP_Header_Type{
        TypeA,TypeB,TypeC,TypeD
    }UOP_Header_Type;
struct UOP_Header{
    UOP_Header_Type type;
    uint8_t flags;
    uint8_t cmd;
    uint32_t src;
    uint32_t dsc;
    uint32_t sn;
    uint16_t check;
    uint16_t data_size;
    uint8_t *data;
};
class UOP
{
private:
    static const string version ;
    /* data */
public:
    UOP(/* args */);
    ~UOP();

    
    string GetVersion();

};
const string UOP::version = "3.0";
#endif