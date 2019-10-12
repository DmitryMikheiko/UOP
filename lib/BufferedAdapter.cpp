#include "BufferedAdapter.h"
void BufferedAdapter::AllocBuffer(BufferType *buffer,int size){
    if(buffer == NULL) return;
    if(buffer->buffer != NULL){

        byte *temp = new byte[size];
        memcpy(temp,buffer->buffer,buffer->size_max);
        delete [] buffer->buffer;
        buffer->buffer = temp;
        buffer->size_max = size;
    }
    else{
            buffer->buffer = new byte[size];
            buffer->size_max = size;
    }
}
void BufferedAdapter::AllocBuffers(int in_size, int out_size){
    AllocBuffer(&input,in_size);
    //AllocBuffer(&output,out_size);
}
void BufferedAdapter::AllocBuffersDefault(){
    input.buffer = NULL;
    //output.buffer = NULL;
    AllocBuffers(MaxBufferSize_Default,MaxBufferSize_Default);
}
void BufferedAdapter::DefaultInit(){
    id = ++id_counter;
    MaxPacketSize = MaxPacketSize_Default;
    IncomePacketDelay = IncomePacketDelay_Default;
    OutcomePacketDelay = OutcomePacketDelay_Default;
    TimeoutDelay = TimeoutDelay_Default;
    StreamWrite = NULL;
    StreamOutEmpty = NULL;
    AllocBuffersDefault();
    input.read_pos = 0;
    input.write_pos = 0;
    input.size = 0;
    //output.read_pos = 0;
    //output.write_pos = 0;
    //output.size = 0;
    LastIncomeTime = 0;
    TxEmpty = true;
    TxCompleteUse = false;
    PacketMode = false;
}
BufferedAdapter::BufferedAdapter(){  
    DefaultInit(); 
}
BufferedAdapter::BufferedAdapter(StreamWriteFunc sw) {   
    DefaultInit(); 
    StreamWrite = sw;
}
BufferedAdapter::BufferedAdapter(StreamWriteFunc sw, StreamOutEmptyFunc soe){
    DefaultInit(); 
    StreamWrite = sw;
    StreamOutEmpty = soe;
}
bool BufferedAdapter::IncomeBufferIsFull(){
    return input.size == input.size_max;
}
bool BufferedAdapter::Write(byte b){
    if(IncomeBufferIsFull()) return true;
    if(PacketMode && IncomePacketAvailable()) FlushIncomeBuffer();
    if(input.write_pos >= input.size_max) input.write_pos = 0;
    input.buffer[input.write_pos++] = b;
    input.size++;
    LastIncomeTime = GetTime_ms();
    return true;
}
int BufferedAdapter::Write(byte *buffer,int size){
    if(size==0 || IncomeBufferIsFull()) return 0;
    if(PacketMode && IncomePacketAvailable()) FlushIncomeBuffer();
    if(size > input.size_max - input.size) size = input.size_max - input.size;
    if(input.write_pos + size <= input.size_max){
        memcpy(&input.buffer[input.write_pos],buffer,size);
        input.write_pos += size;
    }
    else{
        int part1size = input.size_max - input.write_pos;
        if(part1size){
           memcpy(&input.buffer[input.write_pos],buffer,part1size); 
        }
        input.write_pos = 0;
        int part2size = size - part1size;
        memcpy(input.buffer,&buffer[part1size],part2size);
    }
    input.size += size;
    LastIncomeTime = GetTime_ms();
    return size;
}
int BufferedAdapter::ReadAll(byte *buffer,int maxsize){

    int part1size = input.size;
    int part2size = 0;
    if(maxsize < input.size){
        part1size = maxsize;
    }
    if(input.read_pos + part1size <= input.size_max){
        memcpy(buffer, &input.buffer[input.read_pos],part1size);
        input.size -= part1size;
        input.read_pos += part1size;
    }
    else{
        part1size = input.size - input.read_pos;
        if(part1size){
            memcpy(buffer,&input.buffer[input.read_pos],part1size); 
        }
        input.read_pos = 0;
        part2size = input.size - part1size;
        if(part1size + part2size > maxsize) part2size = maxsize - part1size;
        memcpy(&buffer[part1size],input.buffer,part2size);
        input.size -= part2size;
    }      
    return part1size+part2size;
}
void BufferedAdapter::TxComplete(){
    TxEmpty=true;
}
void BufferedAdapter::TxCompleteEnable(){
    TxCompleteUse = true;
    TxEmpty = true;
}
void BufferedAdapter::TxCompleteDisable(){
    TxCompleteUse = false;
    TxEmpty = true;
}
bool BufferedAdapter::WritePacket(byte *buffer,int size){
    int time_s = GetTime_ms();
    int packet_size;
    int transmitted_size;
    int buffer_ptr = 0;
    bool tx_wait = false;
    while(GetTime_ms() - time_s < TimeoutDelay){
        if(StreamOutEmpty != NULL) TxEmpty = StreamOutEmpty();
        if(TxEmpty){
            if(tx_wait){
                tx_wait = false;
                delay_ms(OutcomePacketDelay);
            }
            packet_size = size > MaxPacketSize ? MaxPacketSize : size;
            if(packet_size == 0) return true;
            if(TxCompleteUse || StreamOutEmpty != NULL) TxEmpty = false;
            tx_wait = true;
            transmitted_size = StreamWrite(&buffer[buffer_ptr],packet_size);
            buffer_ptr += transmitted_size;
            size -= transmitted_size;
            if(transmitted_size) time_s = GetTime_ms();
        }
    }
    return false;
}
bool BufferedAdapter::IncomePacketAvailable(){
    return (input.size > 0 && GetTime_ms() - LastIncomeTime > IncomePacketDelay);
}
int BufferedAdapter::ReadPacket(byte *buffer,int maxsize){
    int time_s = GetTime_ms();
    while(GetTime_ms() - time_s < TimeoutDelay){
        if(IncomePacketAvailable()) return ReadAll(buffer,maxsize);
    }
    return 0;
}
void BufferedAdapter::FlushIncomeBuffer(){
    input.size = 0;
    input.write_pos = 0;
    input.read_pos = 0;
}
void BufferedAdapter::PacketModeEnable(){
    PacketMode = true;
}
void BufferedAdapter::PacketModeDisable(){
    PacketMode = false;
}
void BufferedAdapter::SetMaxIncomeBufferSize(int size){
    AllocBuffer(&input,size);
}
void BufferedAdapter::SetMaxPacketSize(int size){
    MaxPacketSize = size;
}
void BufferedAdapter::SetTimeoutDelay(int ms){
    TimeoutDelay = ms;
}
void BufferedAdapter::SetOutcomePacketDelay(int ms){
    OutcomePacketDelay = ms;
}
void BufferedAdapter::SetIncomePacketDelay(int ms){
    IncomePacketDelay = ms;
}
unsigned int BufferedAdapter::GetID(){
    return id;
}
