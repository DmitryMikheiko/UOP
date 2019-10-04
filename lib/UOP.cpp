#include "UOP.h"
UOP::UOP(/* args */)
{
    cout<<"UOP:";
}

UOP::~UOP()
{
}
string UOP::GetVersion(){
    return version;
}