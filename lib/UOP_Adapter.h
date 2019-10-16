/**
 * @brief This class designed for UOP protol. Allows you to connect any type of data stream to UOP Core.
 * Oct.12,2019
 */
#ifndef _UOP_Adapter_
#define _UOP_Adapter_
#include "UOP.h"
#include "UOP_Buffer.h"
#include "UOP_CommonTypes.h"


#define TimeoutDelay_Default 50
#define MaxBufferSize_Default 11
#define MaxPacketSize_Default 10
#define OutcomePacketDelay_Default 5
#define IncomePacketDelay_Default 5
#define byte unsigned char
struct BufferType{   
    bool lock;
    byte *buffer;
    int size_max;
    int size;
    int read_pos;
    int write_pos;
    long long LastIncomeTime;
    long long LastOutcomeTime;
    BufferType(){
        lock = false;
        buffer = NULL;
        size_max = 0;
        size = 0;
        read_pos = 0;
        write_pos = 0;
        LastIncomeTime = 0;
        LastOutcomeTime = 0;
    }
    void Write(byte *data, int size){
        if(this->size_max > 0 && buffer != NULL) delete [] buffer;
        buffer = new byte[size];
        size_max = size;
        this->size = size;
        memcpy(buffer,data,size);
        write_pos = 0;
        read_pos = 0;
        LastOutcomeTime = 0;
        LastIncomeTime = 0;
    }
};

class UOP_Adapter
{
private:
    static unsigned int id_counter;
    unsigned int id;
    int MaxPacketSize;
    int IncomePacketDelay;
    int OutcomePacketDelay;
    int TimeoutDelay;
    bool TxEmpty;
    bool TxCompleteUse;
    bool PacketMode;
    bool StreamWriteRunning;
    bool OutcomePacketDelayWait;
    bool WriteStreamWait;
    BufferType input;
    BufferType output;
public:
    typedef int (*StreamWriteFunc)(byte *buffer,int size);
    typedef bool (*StreamOutEmptyFunc)();
private:
    StreamWriteFunc StreamWrite;
    StreamOutEmptyFunc StreamOutEmpty;

    #define InputLocked input.lock
    #define OutputLocked output.lock
    #define LockInput input.lock = true;
    #define LockOutput output.lock = true;
    
    void AllocBuffer(BufferType *buffer,int size);
    void AllocBuffers(int in_size, int out_size);
    void AllocBuffersDefault();
    UOP_StateType WritePacketAsync();
public:
    void DefaultInit();
    UOP_Adapter();
    UOP_Adapter(StreamWriteFunc sw);
    UOP_Adapter(StreamWriteFunc sw, StreamOutEmptyFunc soe);
    ~UOP_Adapter();
    // Stream-side methods:
    bool Write(byte b);
    int  Write(byte *buffer,int size);
    void TxComplete();
    void TxCompleteEnable();
    void TxCompleteDisable();
    bool IncomeBufferIsFull();
    // UOP_side methods:
    //protected:
    int  ReadAll(byte *buffer, int maxsize);
    int  ReadPacket(byte *buffer, int maxsize);
    UOP_StateType WritePacket(byte *buffer, int size, bool async = false);
    UOP_StateType WritePacketCompleteCheck();
    bool IncomePacketAvailable();
    void FlushIncomeBuffer();
    // Settings:
    void SetStreamWriteFunc(StreamWriteFunc sw);
    void SetStreamOutEmptyFunc(StreamOutEmptyFunc soe);
    void PacketModeEnable();
    void PacketModeDisable();
    void SetMaxIncomeBufferSize(int size);
    void SetMaxPacketSize(int size);
    void SetTimeoutDelay(int ms);
    void SetOutcomePacketDelay(int ms);
    void SetIncomePacketDelay(int ms);
    // ID
    unsigned int GetID();
};
unsigned int UOP_Adapter::id_counter = 0;
#endif