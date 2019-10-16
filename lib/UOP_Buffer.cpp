/**
 *  UOP_Buffer
 */
#include "UOP_Buffer.h"
UOP_Buffer::UOP_Buffer(){
    buffer = NULL;
    buffer_size = 0;
    size = 0;
}
UOP_Buffer::~UOP_Buffer(){
    if(buffer != NULL) delete [] buffer;
}
void UOP_Buffer::Resize(int size,bool shrink){
    if(size > buffer_size || (size < buffer_size && shrink)){
        delete [] buffer;
        buffer = new byte[size];
        buffer_size = size > buffer_size ? size : buffer_size;
    }
    this->size = size;
}
void UOP_Buffer::Write(byte *data,int size, bool shrink){
    Resize(size,shrink);
    memcpy(buffer,data,size);
    this->size = size;
}
byte* UOP_Buffer::GetBuffer(){
    return buffer;
}
int UOP_Buffer::GetSize(){
    return size;
}