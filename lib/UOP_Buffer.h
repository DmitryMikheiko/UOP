#ifndef __UOP_Buffer__
#define __UOP_Buffer__
#include <string>
#define byte unsigned char
class UOP_Buffer{
    int  buffer_size;
    byte *buffer;
    int  size;
public:
    UOP_Buffer();
    ~UOP_Buffer();
    void Resize(int size, bool shrink = true);
    void Write(byte *data,int size, bool shrink = false);
    byte* GetBuffer();
    int GetSize();
};
#endif