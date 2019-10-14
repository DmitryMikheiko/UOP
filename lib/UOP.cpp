#include "UOP.h"

UOP::UOP(){
    AdaptersVector.reserve(UOP_AdaptersList_Max_Size);
    UOP_Frame frame;
    UOP_Packet packet;
    frame.type = TypeB;
    frame.data_size = 10;
    codec.EncodeFrame(frame,packet);
}
UOP::~UOP(){
    for(int i=0;i<AdaptersVector.size();i++) {
      AdaptersVector[i]->~BufferedAdapter();
    }
}
vector<BufferedAdapter*>::iterator UOP::GetAdapterIterator(BufferedAdapter *adapter){
    vector<BufferedAdapter*>::iterator adapters_it;
    adapters_it = AdaptersVector.begin();
    while(adapters_it != AdaptersVector.end()) 
        if(*adapters_it++ == adapter) break;
    return adapters_it;
}
int UOP::GetAdapterIndex(BufferedAdapter *adapter){
    if(adapter == NULL) return -1;
    for(int i=0;i<AdaptersVector.size();i++) 
        if(AdaptersVector[i] == adapter) return i;
    return -1;
}
bool UOP::AddAdapter(BufferedAdapter *adapter){
    if(adapter == NULL && AdaptersVector.size() == AdaptersVector.capacity()) return false;
    AdaptersVector.push_back(adapter);
    //cout<<"id: "<<adapter->GetID()<<endl;
    //cout<<"size: "<<AdaptersVector.size()<<endl;
    return true;
}
BufferedAdapter* UOP::AddNewAdapter(){
    BufferedAdapter* adapter = new BufferedAdapter();
    if(AddAdapter(adapter)) return adapter;
    return NULL;
}
bool UOP::RemoveAdapter(BufferedAdapter *adapter){
    int i = GetAdapterIndex(adapter);
    if(i == -1) return false;
    AdaptersVector.erase(AdaptersVector.begin() + i);
    //cout<<"size: "<<AdaptersVector.size()<<endl;
    return true;
}
string UOP::GetVersion(){
    return version;
}
     