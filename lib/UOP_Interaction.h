#ifndef __UOP_INTERACTION__
#define __UOP_INTERACTION__
#include "UOP_CommonTypes.h"
#include "UOP_Adapter.h"

class UOP_Interaction
{
private:
UOP_Adapter *adapter;
public:
    UOP_Interaction(UOP_Adapter *adapter);
    ~UOP_Interaction();
};
#endif