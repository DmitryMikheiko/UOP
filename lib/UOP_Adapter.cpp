#include "UOP_Adapter.h"
void UOP_Adapter::AllocBuffer(BufferType *buffer,int size){
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
void UOP_Adapter::AllocBuffers(int in_size, int out_size){
    AllocBuffer(&input,in_size);
    //AllocBuffer(&output,out_size);
}
void UOP_Adapter::AllocBuffersDefault(){
    input.buffer = NULL;
    //output.buffer = NULL;
    AllocBuffers(MaxBufferSize_Default,0);
}
void UOP_Adapter::DefaultInit(){
    id = ++id_counter;
    MaxPacketSize = MaxPacketSize_Default;
    IncomePacketDelay = IncomePacketDelay_Default;
    OutcomePacketDelay = OutcomePacketDelay_Default;
    TimeoutDelay = TimeoutDelay_Default;
    StreamWrite = NULL;
    StreamOutEmpty = NULL;
    AllocBuffersDefault();
    TxEmpty = true;
    TxCompleteUse = false;
    PacketMode = false;
    StreamWriteRunning = false;
    OutcomePacketDelayWait = false;
    WriteStreamWait = false;
}
UOP_Adapter::UOP_Adapter(){  
    DefaultInit(); 
}
UOP_Adapter::UOP_Adapter(StreamWriteFunc sw) {   
    DefaultInit(); 
    StreamWrite = sw;
}
UOP_Adapter::UOP_Adapter(StreamWriteFunc sw, StreamOutEmptyFunc soe){
    DefaultInit(); 
    StreamWrite = sw;
    StreamOutEmpty = soe;
}
UOP_Adapter::~UOP_Adapter(){
    if(input.buffer != NULL) delete [] input.buffer;
    if(output.buffer != NULL) delete [] output.buffer;
    //cout<<"BufferedAdapter "<<id<<" deleted"<<endl;
}
bool UOP_Adapter::IncomeBufferIsFull(){
    return input.size == input.size_max;
}
bool UOP_Adapter::Write(byte b){
    if(IncomeBufferIsFull()) return true;
    if(PacketMode && IncomePacketAvailable()) FlushIncomeBuffer();
    if(input.write_pos >= input.size_max) input.write_pos = 0;
    input.buffer[input.write_pos++] = b;
    input.size++;
    input.LastIncomeTime = GetTime_ms();
    return true;
}
int UOP_Adapter::Write(byte *buffer,int size){
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
    input.LastIncomeTime = GetTime_ms();
    return size;
}
int UOP_Adapter::ReadAll(byte *buffer,int maxsize){

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
void UOP_Adapter::TxComplete(){
    TxEmpty=true;
}
void UOP_Adapter::TxCompleteEnable(){
    TxCompleteUse = true;
    TxEmpty = true;
}
void UOP_Adapter::TxCompleteDisable(){
    TxCompleteUse = false;
    TxEmpty = true;
}
UOP_StateType UOP_Adapter::WritePacket(byte *buffer,int size, bool async){
    if(StreamWrite == NULL) return UOP_ERROR;
    if(StreamWriteRunning == true) return UOP_BUSY;
    int time_s = GetTime_ms();
    int packet_size;
    int transmitted_size;
    int buffer_ptr = 0;
    bool tx_wait = false;
    UOP_StateType state;
    StreamWriteRunning = true;
    output.Write(buffer,size);
    output.read_pos = 0;
    if(!async){
        do{
            state = WritePacketAsync();
            if(state == UOP_OK) return UOP_OK;
        }while(state == UOP_RUNNING);
        return state;
    }
    return UOP_OK;
}
UOP_StateType UOP_Adapter::WritePacketAsync(){
    if(!StreamWriteRunning) return UOP_OK;
    long long time_now = GetTime_ms();
    if(OutcomePacketDelayWait && time_now - output.LastOutcomeTime < OutcomePacketDelay) return UOP_RUNNING;
    OutcomePacketDelayWait = false;
    if(WriteStreamWait && !TxEmpty){
        if((time_now - output.LastOutcomeTime + OutcomePacketDelay) >= TimeoutDelay) {
            StreamWriteRunning = false;
            WriteStreamWait = false;
            return UOP_WRITE_TIMEOUT;
        }
        if(StreamOutEmpty != NULL)
            if(!StreamOutEmpty()){
                return UOP_RUNNING;
            }
            else TxEmpty = true;
        else return UOP_RUNNING;
    } 
    if(WriteStreamWait){
        WriteStreamWait = false;
        if(OutcomePacketDelay > 0) {
            OutcomePacketDelayWait = true;
            return UOP_RUNNING;
        }
    }
    if(TxEmpty){
        int packet_size = output.size > MaxPacketSize ? MaxPacketSize : output.size;
        if(packet_size == 0) {
            StreamWriteRunning = false;
            return UOP_OK;
        }
        if(TxCompleteUse || StreamOutEmpty != NULL) TxEmpty = false;
        WriteStreamWait = true;
        int transmitted_size = StreamWrite(&output.buffer[output.read_pos],packet_size);
        output.read_pos += transmitted_size;
        output.size -= transmitted_size;
        if(transmitted_size) output.LastOutcomeTime = GetTime_ms();
        if(output.size == 0 && OutcomePacketDelay == 0){
            StreamWriteRunning = false;
            return UOP_OK;
        }
    }
    else WriteStreamWait = true;
    return UOP_RUNNING;
}
UOP_StateType UOP_Adapter::WritePacketCompleteCheck(){  
    return WritePacketAsync();
}
bool UOP_Adapter::IncomePacketAvailable(){
    return (input.size > 0 && GetTime_ms() - input.LastIncomeTime > IncomePacketDelay);
}
int UOP_Adapter::ReadPacket(byte *buffer,int maxsize){
    int time_s = GetTime_ms();
    while(GetTime_ms() - time_s < TimeoutDelay){
        if(IncomePacketAvailable()) return ReadAll(buffer,maxsize);
    }
    return 0;
}
void UOP_Adapter::FlushIncomeBuffer(){
    input.size = 0;
    input.write_pos = 0;
    input.read_pos = 0;
}
void UOP_Adapter::SetStreamWriteFunc(StreamWriteFunc sw){
     StreamWrite = sw;
}
void UOP_Adapter::SetStreamOutEmptyFunc(StreamOutEmptyFunc soe){
    StreamOutEmpty = soe;
}
void UOP_Adapter::PacketModeEnable(){
    PacketMode = true;
}
void UOP_Adapter::PacketModeDisable(){
    PacketMode = false;
}
void UOP_Adapter::SetMaxIncomeBufferSize(int size){
    AllocBuffer(&input,size);
}
void UOP_Adapter::SetMaxPacketSize(int size){
    MaxPacketSize = size;
}
void UOP_Adapter::SetTimeoutDelay(int ms){
    TimeoutDelay = ms;
}
void UOP_Adapter::SetOutcomePacketDelay(int ms){
    OutcomePacketDelay = ms;
}
void UOP_Adapter::SetIncomePacketDelay(int ms){
    IncomePacketDelay = ms;
}
unsigned int UOP_Adapter::GetID(){
    return id;
}
