#include <iostream>
#include "lib\UOP.cpp"
#include "lib\BufferedAdapter.cpp"

#include <chrono>
#include <random>
using namespace std;

#define arrs_size 10
#define _time_ PrintTime();
#define _arrscmp_ CompareArrays(arr1,arr2,arrs_size);
#define _a1print_ PrintArray(arr1,arrs_size);
#define _a2print_ PrintArray(arr2,arrs_size);

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(0,255);

void PrintArray(byte *arr,int size);
void PrintTime();
int SerialWrite(byte *buffer, int size);
bool SerialTxEmpty();
BufferedAdapter bufferedAdapter(SerialWrite,SerialTxEmpty);  

void PrintArray(byte *arr,int size){
    cout<<GetTime_ms()<<" ms: "<<size<<" items "<<endl;
    for(int i=0;i<size;i++) cout<<(int)arr[i]<<", ";
    cout<<endl;
}
void PrintTime(){
    cout<<GetTime_ms()<<" ms"<<endl;
}
void CompareArrays(byte *arr1,byte *arr2, int size){
    if(memcmp(arr1,arr2,size) == 0) cout<<"Arrays are equal"<<endl;
    else cout<<"Arrays are NOT equal"<<endl;
}
int SerialWrite(byte *buffer, int size){
    cout<<"SerialWrite: ";
    PrintArray(buffer,size);
    //delay_ms(10);
    //bufferedAdapter.TxComplete();
    return size;
}
bool SerialTxEmpty(){
    return true;
}
void RandomInitBuffer(byte *buffer,int size){
    do{
        buffer[--size] = distribution(generator);;
    }while(size);
}
int main(){
    UOP uop;
    
    InitialTime = Time::now();
  
    byte *arr1 = new byte[arrs_size];
    byte *arr2 = new byte[arrs_size];
    byte *buf = new byte[1000];
    int n;

    RandomInitBuffer(arr1,arrs_size);
    RandomInitBuffer(arr2,arrs_size);
    
    BufferedAdapter b1;
    BufferedAdapter b2;
    cout<<"id1: "<<b1.GetID()<<endl;
    cout<<"id2: "<<b2.GetID()<<endl;
    cin.get();
    return 0;
}