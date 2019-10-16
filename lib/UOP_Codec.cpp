#include "UOP_Codec.h"

/**
 * UOP_MultitypeConverter
 * */
void UOP_MultitypeConverter::WriteU32toArray(uint32_t data,byte *arr,int &offset){
    memcpy(arr + offset,(const void *)&data,4);
    offset += 4;
}
void UOP_MultitypeConverter::WriteU16toArray(uint16_t data, byte *arr,int &offset){
    memcpy(arr + offset,(const void *)&data,2);
    offset += 2;
}
void UOP_MultitypeConverter::WriteValueToArray(uint32_t data, UOP_Variable_Type type, byte *arr, int &offset){
    switch (type)
    {
    case U8:
        arr[offset++] = (byte) data;
        break;
    case U16:
        WriteU16toArray((uint16_t)data,arr,offset);
        break;
    case U32:
        WriteU32toArray(data,arr,offset);
        break;
    default:
        break;
    }
}
uint32_t UOP_MultitypeConverter::ReadU32fromArray(byte *arr, int &offset){
    uint32_t data;
    memcpy((void *)&data,arr + offset, 4);
    offset += 4;
    return data;
}
uint16_t UOP_MultitypeConverter::ReadU16fromArray(byte *arr, int &offset){
    uint16_t data;
    memcpy((void *)&data,arr + offset, 2);
    offset += 2;
    return data;
}
uint32_t UOP_MultitypeConverter::ReadValueFromArray(byte *arr, int &offset, UOP_Variable_Type type){
    switch (type)
    {
    case U8:
        return arr[offset++];
    case U16:
        return ReadU16fromArray(arr,offset);   
    case U32:
        return ReadU32fromArray(arr,offset);      
    default:
        return 0;
    }
}
/**
 *  UOP_Codec
 * */

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
UOP_Variable_Type UOP_Codec::GetChecksumVariableType(UOP_Checksum_Type type){
    switch (type)
    {
    case XOR:
        return U8;
    case CRC8:
        return U8;
    case CRC16:
        return U16;
    case CRC32:
        return U32;
    default:
        return NONE;
    }
}
int UOP_Codec::GetFrameSize(UOP_Frame &frame){
    int size = 0;
    UOP_Header_Def header_def = GetHeaderDef(frame.type);
    size += GetHeaderSize(header_def);
    size += GetTypeSize(header_def.data) * frame.data_size;
    return size;
}
int UOP_Codec::GetHeaderSize(UOP_Header_Def header, bool marker_disable){
    int size = 0;
    if(!marker_disable) size += GetTypeSize(header.marker);
    size += GetTypeSize(header.flags);
    size += GetTypeSize(header.cmd);
    size += GetTypeSize(header.src);
    size += GetTypeSize(header.dsc);
    size += GetTypeSize(header.sn);
    size += GetTypeSize(header.data_size);
    size += GetChecksumSize(header.checksum);
    return size;
}
byte UOP_Codec::GetMarkerValue(UOP_Header_Type type){
   switch (type)
    {
    case TypeA: return UOP_Marker_Type_A;
    case TypeB: return UOP_Marker_Type_B;
    case TypeC: return UOP_Marker_Type_C;
    case TypeD: return UOP_Marker_Type_D;
    default:
        return 0;
    }
}
bool UOP_Codec::EncodeFrame(UOP_Frame &frame, UOP_Packet &packet){
    int size = GetFrameSize(frame);
    cout<<">Encoder:"<<endl<<" size: "<<size<<endl;
    packet.buffer.Resize(size);
    byte *buffer = packet.buffer.GetBuffer();
    if(buffer == NULL) return false;
    int ptr = 0;
    int checksum_ptr;
    int checksum_offset = 0;
    UOP_Header_Def header_def = GetHeaderDef(frame.type);
    // Marker
    switch (header_def.marker)
    {
    case U8:
        buffer[ptr++] = (byte)frame.type;
        break;
    case U16:
        checksum_offset = 1;
        buffer[ptr++] = (byte)frame.type;
        buffer[ptr++] = (byte)~frame.type;
        break;  
    case U32:
        checksum_offset = 3;
        buffer[ptr++] = UOP_Marker_Preambule;
        buffer[ptr++] = UOP_Marker_Preambule;
        buffer[ptr++] = (byte)frame.type;
        buffer[ptr++] = (byte)~frame.type ;
        break;
    default:
        break;
    }
    // FLAGS
    if(frame.data_size == 0) frame.flags |= UOP_Flags_DZ_Mask;
    frame.flags |= (byte)header_def.checksum;
    UOP_MultitypeConverter::WriteValueToArray(frame.flags, header_def.flags, buffer, ptr);
    // CMD
    UOP_MultitypeConverter::WriteValueToArray(frame.cmd, header_def.cmd, buffer, ptr);
    // SRC
    UOP_MultitypeConverter::WriteValueToArray(frame.src, header_def.src, buffer, ptr);
    // DSC
    UOP_MultitypeConverter::WriteValueToArray(frame.dsc, header_def.dsc, buffer, ptr);
    // SN
    UOP_MultitypeConverter::WriteValueToArray(frame.sn, header_def.sn, buffer, ptr);
    // CHECKSUM
    checksum_ptr = ptr;
    UOP_MultitypeConverter::WriteValueToArray(0x00,GetChecksumVariableType(header_def.checksum), buffer, ptr);
    // DATA_SIZE && DATA
    if(frame.data_size != 0 && frame.data.GetSize() >= frame.data_size){
        UOP_MultitypeConverter::WriteValueToArray(frame.data_size, header_def.data_size, buffer, ptr); 
        memcpy(buffer + ptr, frame.data.GetBuffer(), frame.data_size);
    }
    //CHECKSUM
    UOP_MultitypeConverter::WriteValueToArray(GetChecksum(buffer, checksum_offset,size - checksum_offset, header_def.checksum), 
                                            GetChecksumVariableType(header_def.checksum), buffer, checksum_ptr);

    return true;
}
bool UOP_Codec::IsValueMarker(byte value){
   return (value == UOP_Marker_Type_A || value == UOP_Marker_Type_B || value == UOP_Marker_Type_C || value == UOP_Marker_Type_D);
}
bool UOP_Codec::DecodePacket(UOP_Packet &packet, UOP_Frame &frame, UOP_Variable_Type marker_min_requirements){
    int size = packet.buffer.GetSize();
    if(size < 4) return false;
    byte *buffer = packet.buffer.GetBuffer();
    UOP_Variable_Type marker_type; 
    UOP_Header_Def header_def;
    int ptr = 0;
    int checksum_offset = 0;
    int checksum_ptr;
    byte marker_value;

    for(;ptr < 4;ptr++) 
        if(buffer[ptr] != UOP_Marker_Preambule) break;
    if(ptr >= 2 && buffer[2] == (byte)~buffer[3] && IsValueMarker(buffer[2])) {
        marker_type = U32; 
        marker_value = buffer[2];
        checksum_offset = 3;
        ptr = 4;
    }
    else if(buffer[0] == (byte)~buffer[1] && IsValueMarker(buffer[0])) {
        marker_type = U16;
        marker_value = buffer[0];
        checksum_offset = 1;
        ptr = 2;
    }
    else if(IsValueMarker(buffer[0])) {
        marker_type = U8;
        marker_value = buffer[0];
        ptr = 1;
    }
    else return false;
    if(marker_type < marker_min_requirements) return false;
    header_def = GetHeaderDef((UOP_Header_Type)marker_value);
    if(size - ptr < GetHeaderSize(header_def,true)) return false;
    // FLAGS
    frame.flags = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,header_def.flags);
    UOP_Checksum_Type checksum_type = (UOP_Checksum_Type) (frame.flags & UOP_Flags_CS_Mask);
    // CMD
    frame.cmd = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,header_def.cmd);
    // SRC
    frame.src = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,header_def.src);
    // DSC
    frame.dsc = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,header_def.dsc);
    // SN
    frame.sn = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,header_def.sn);
    // CHECKSUM
    checksum_ptr = ptr;
    int checksum = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,GetChecksumVariableType(header_def.checksum));
    for(;checksum_ptr < ptr;) buffer[checksum_ptr++] = 0;
    if(GetChecksum(buffer,checksum_offset,size - checksum_offset,checksum_type) != checksum) return false;
    if(!(frame.flags & UOP_Flags_DZ_Mask)){
        frame.data_size = UOP_MultitypeConverter::ReadValueFromArray(buffer,ptr,header_def.data_size);
        if(size - ptr < frame.data_size) return false;
        frame.data.Resize(frame.data_size);
        memcpy(frame.data.GetBuffer(),buffer + ptr,frame.data_size);
    }
    else frame.data_size = 0;
    return true;
}
byte UOP_Codec::GetChecksumXOR(byte *buffer, int size){
    if(buffer == NULL) return 0xFF;
    byte cs = 0;
    for(int n = 0; n < size;) cs ^= buffer[n++];
    return cs;
}
uint32_t UOP_Codec::GetChecksum(byte *buffer, int offset, int size, UOP_Checksum_Type type){
    switch (type)
    {
    case XOR:
        return GetChecksumXOR(buffer + offset, size);
    case CRC8:
        return crc_8(buffer + offset, size);
    case CRC16:
        return crc_16(buffer + offset, size);
    case CRC32:
        return crc_32(buffer + offset, size);
    default:
        return 0;
    }
}
