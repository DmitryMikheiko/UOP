/**
 * @brief This class designed for UOP protol. Allows you to connect any type of data stream to UOP Core.
 * Oct.12,2019
 */
#ifndef _BufferedAdapter_
#define _BufferedAdapter_
#include "UOP.h"
#define TimeoutDelay_Default 1000
#define MaxBufferSize_Default 11
#define MaxPacketSize_Default 20
#define OutcomePacketDelay_Default 0
#define IncomePacketDelay_Default 5
#define byte unsigned char

class BufferedAdapter
{
private:
    static unsigned int id_counter;
    unsigned int id;
    int MaxPacketSize;
    int IncomePacketDelay;
    int OutcomePacketDelay;
    int TimeoutDelay;
    long long LastIncomeTime;
    bool TxEmpty;
    bool TxCompleteUse;
    bool PacketMode;
    struct BufferType{
        bool lock;
        byte *buffer;
        int size_max;
        int size;
        int read_pos;
        int write_pos;
    };
    BufferType input;
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
public:
    void DefaultInit();
    BufferedAdapter();
    BufferedAdapter(StreamWriteFunc sw);
    BufferedAdapter(StreamWriteFunc sw, StreamOutEmptyFunc soe);
    // Stream-side methods:
    bool Write(byte b);
    int  Write(byte *buffer,int size);
    void TxComplete();
    void TxCompleteEnable();
    void TxCompleteDisable();
    bool IncomeBufferIsFull();
    // UOP_side methods:
    //protected:
    int  ReadAll(byte *buffer,int maxsize);
    bool WritePacket(byte *buffer,int size);
    bool IncomePacketAvailable();
    int  ReadPacket(byte *buffer,int maxsize);
    void FlushIncomeBuffer();
    // Settings:
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
unsigned int BufferedAdapter::id_counter = 0;
#endif