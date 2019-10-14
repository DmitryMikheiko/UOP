#include "UOP_Codec.h"

UOP_Packet::UOP_Packet(){
    buffer = NULL;
    buffer_size = 0;
    size = 0;
}
UOP_Packet::~UOP_Packet(){
    if(buffer != NULL) delete [] buffer;
}
void UOP_Packet::Resize(int size,bool shrink = false){
    if(size > buffer_size || (size < buffer_size && shrink)){
        delete [] buffer;
        buffer = new byte[size];
        buffer_size = size > buffer_size ? size : buffer_size;
    }
}
void UOP_Packet::Write(byte *data,int size){
    Resize(size);
    memcpy(buffer,data,size);
    this->size = size;
}
byte* UOP_Packet::GetBuffer(){
    return buffer;
}
int UOP_Packet::GetSize(){
    return size;
}
UOP_Codec::UOP_Codec(){
    init_crc16_tab();
    init_crc32_tab();
}
UOP_Codec::~UOP_Codec(){

}
int UOP_Codec::GetTypeSize(UOP_Variable_Type type){
    switch (type)
    {
    case U8:
        return 1;
    case U16:
        return 2;
    case U32:
        return 4;
    default:
        return 0;
    }
}
int UOP_Codec::GetChecksumSize(UOP_Checksum_Type type){
    switch (type)
    {
    case XOR:
        return 1;
    case CRC8:
        return 1;
    case CRC16:
        return 2;
    case CRC32:
        return 4;
    default:
        return 0;
    }
}
UOP_Header_Def UOP_Codec::GetHeaderDef(UOP_Header_Type type){
    switch (type)
    {
    case TypeA: return UOP_Header_A;
    case TypeB: return UOP_Header_B;
    case TypeC: return UOP_Header_C;
    case TypeD: return UOP_Header_D;
    default:
        return UOP_Header_A;
    }
}
int UOP_Codec::GetPacketSize(UOP_Frame &frame){
    int size=0;
    UOP_Header_Def header_def = GetHeaderDef(frame.type);
    size += GetTypeSize(header_def.marker);
    size += GetTypeSize(header_def.flags);
    size += GetTypeSize(header_def.cmd);
    size += GetTypeSize(header_def.src);
    size += GetTypeSize(header_def.dsc);
    size += GetTypeSize(header_def.sn);
    size += GetTypeSize(header_def.data_size);
    size += GetTypeSize(header_def.data) * frame.data_size;
    size += GetChecksumSize(header_def.checksum);
    return size;
}
bool UOP_Codec::EncodeFrame(UOP_Frame &frame, UOP_Packet &packet){
    int size = GetPacketSize(frame);
    cout<<">Encoder:"<<endl<<" size: "<<size<<endl;
    packet.Resize(size);
    byte *buffer = packet.GetBuffer();
    int ptr=0;
    UOP_Header_Def header_def = GetHeaderDef(frame.type);
    switch (header_def.marker)
    {
    case U8:
        buffer[ptr++] = (byte)frame.cmd;
        break;
    case U16:
        buffer[ptr++] = (byte)frame.cmd;
        buffer[ptr++] = !(byte)frame.cmd;
        break;
    
    default:
        break;
    }
    return true;
}
bool UOP_Codec::DecodePacket(UOP_Packet &packet, UOP_Frame &frame){
    return true;
}
byte UOP_Codec::CheckSum(byte *buffer,int offset,int size){
    if(buffer == NULL) return 0xFF;
    byte cs=0;
    for(int n=0;n<size;) cs^=buffer[n];
    return cs;
}
