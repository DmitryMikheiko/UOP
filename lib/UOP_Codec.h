#ifndef __UOP_Codec__
#define __UOP_Codec__
#include "libcrc\checksum.h"
#include "libcrc\crc8.c"
#include "libcrc\crc16.c"
#include "libcrc\crc32.c"
#include "UOP_Buffer.cpp"
#include <string>
#include <iostream>

#define byte unsigned char

using namespace std;

#define UOP_Marker_Preambule (byte)0xAA
#define UOP_Marker_Type_A   (byte)0xAA
#define UOP_Marker_Type_B   (byte)0xA2
#define UOP_Marker_Type_C   (byte)0x2A
#define UOP_Marker_Type_D   (byte)0x22

#define UOP_Flags_DZ_Mask   (byte)0x80  // Bit7. DataZero.              '1' if data_size == 0
#define UOP_Flags_Z_Mask    (byte)0x40  // Bit6. Zero.                  always '0'for guaranteed marker decoding
#define UOP_Flags_ACK_Mask  (byte)0x20  // Bit5. Acknowledge.           '1' if frame is acknowledge of receipt
#define UOP_Flags_ST_Mask   (byte)0x10  // Bit4. Start.                 '1' if frame is first in transaction
#define UOP_Flags_SP_Mask   (byte)0x8   // Bit3. Stop.                  '1' if frame is last in transaction
#define UOP_Flags_AR_Mask   (byte)0x4   // Bit2. Acknowledge required.  '1' if recipient must respond to this frame
#define UOP_Flags_CS_Mask   (byte)0x3   // Bits[1:0]. Checksum. equal to (byte)UOP_Checksum_Type 

#define UOP_Cmd_SC_Mask     (byte)0x80  // Bit7. System command.        '1' if cmd variable is system command value


typedef enum UOP_Header_Type{
    TypeA = UOP_Marker_Type_A,
    TypeB = UOP_Marker_Type_B,
    TypeC = UOP_Marker_Type_C,
    TypeD = UOP_Marker_Type_D
}UOP_Header_Type;
typedef enum UOP_Checksum_Type{
    XOR,    // xor of all bytes
    CRC8,   // CRC-8/MAXIM
    CRC16,  // CRC-16/ARC
    CRC32   // CRC-32/zlib
}UOP_Checksum_Type;
typedef enum UOP_Variable_Type{
    NONE,
    U8,
    U16,
    U32
}UOP_Variable_Type;
struct UOP_Header_Def{
    UOP_Checksum_Type checksum;
    UOP_Variable_Type marker;
    UOP_Variable_Type flags;
    UOP_Variable_Type cmd;
    UOP_Variable_Type src;
    UOP_Variable_Type dsc;
    UOP_Variable_Type sn;
    UOP_Variable_Type data_size;
    UOP_Variable_Type data;
};
const UOP_Header_Def UOP_Header_A = {
    XOR,    // (UOP_Checksum_Type) checksum method 
    U32,    // (UOP_Variable_Type) marker   
    U8,     // (UOP_Variable_Type) flags    
    U8,     // (UOP_Variable_Type) cmd      
    U8,     // (UOP_Variable_Type) src      
    U8,     // (UOP_Variable_Type) dsc      
    U8,     // (UOP_Variable_Type) sn       
    NONE,   // (UOP_Variable_Type) data_size 
    NONE    // (UOP_Variable_Type) data     
};
const UOP_Header_Def UOP_Header_B = {
    CRC8,   // (UOP_Checksum_Type) checksum method 
    U8,     // (UOP_Variable_Type) marker   
    U8,     // (UOP_Variable_Type) flags    
    U8,     // (UOP_Variable_Type) cmd      
    U8,     // (UOP_Variable_Type) src      
    U8,     // (UOP_Variable_Type) dsc      
    U8,     // (UOP_Variable_Type) sn       
    U8,     // (UOP_Variable_Type) data_size 
    U8      // (UOP_Variable_Type) data     
};
UOP_Header_Def UOP_Header_C;
UOP_Header_Def UOP_Header_D;

struct UOP_Frame{
    UOP_Header_Type type;
    uint32_t flags;
    uint32_t cmd;
    uint32_t src;
    uint32_t dsc;
    uint32_t sn;
    uint32_t data_size;
    UOP_Buffer data;
};

struct UOP_Packet
{
    UOP_Buffer buffer;
};
class UOP_MultitypeConverter{
    public:
static void WriteU32toArray(uint32_t data, byte *arr,int &offset);
static void WriteU16toArray(uint16_t data, byte *arr,int &offset);
static void WriteValueToArray(uint32_t data, UOP_Variable_Type type, byte *arr, int &offset);
static uint32_t ReadU32fromArray(byte *arr, int &offset);
static uint16_t ReadU16fromArray(byte *arr, int &offset);
static uint32_t ReadValueFromArray(byte *arr, int &offset, UOP_Variable_Type type);
};

class UOP_Codec
{   

public:
    UOP_Codec();
    ~UOP_Codec();
    int GetTypeSize(UOP_Variable_Type type);
    int GetChecksumSize(UOP_Checksum_Type type);
    UOP_Header_Def GetHeaderDef(UOP_Header_Type type);
    UOP_Variable_Type GetChecksumVariableType(UOP_Checksum_Type type);
    byte GetMarkerValue(UOP_Header_Type type);
    int GetFrameSize(UOP_Frame &frame);
    int GetHeaderSize(UOP_Header_Def header, bool marker_disable = false);
inline bool IsValueMarker(byte value);
    bool EncodeFrame(UOP_Frame &frame, UOP_Packet &packet);
    bool DecodePacket(UOP_Packet &packet, UOP_Frame &frame,UOP_Variable_Type marker_min_requirements = NONE);
    byte GetChecksumXOR(byte *buffer, int size);
    uint32_t GetChecksum(byte *buffer, int offset, int size, UOP_Checksum_Type type);
};

#endif 