#include "UOP.h"

UOP::UOP(){
    AdaptersVector.reserve(UOP_AdaptersList_Max_Size);
}
UOP::~UOP(){
    for(int i=0;i<AdaptersVector.size();i++) {
      AdaptersVector[i]->~UOP_Adapter();
    }
}
vector<UOP_Adapter*>::iterator UOP::GetAdapterIterator(UOP_Adapter *adapter){
    vector<UOP_Adapter*>::iterator adapters_it;
    adapters_it = AdaptersVector.begin();
    while(adapters_it != AdaptersVector.end()) 
        if(*adapters_it++ == adapter) break;
    return adapters_it;
}
int UOP::GetAdapterIndex(UOP_Adapter *adapter){
    if(adapter == NULL) return -1;
    for(int i=0;i<AdaptersVector.size();i++) 
        if(AdaptersVector[i] == adapter) return i;
    return -1;
}
bool UOP::AddAdapter(UOP_Adapter *adapter){
    if(adapter == NULL && AdaptersVector.size() == AdaptersVector.capacity()) return false;
    AdaptersVector.push_back(adapter);
    //cout<<"id: "<<adapter->GetID()<<endl;
    //cout<<"size: "<<AdaptersVector.size()<<endl;
    return true;
}
UOP_Adapter* UOP::AddNewAdapter(){
    UOP_Adapter* adapter = new UOP_Adapter();
    if(AddAdapter(adapter)) return adapter;
    return NULL;
}
bool UOP::RemoveAdapter(UOP_Adapter *adapter){
    int i = GetAdapterIndex(adapter);
    if(i == -1) return false;
    AdaptersVector.erase(AdaptersVector.begin() + i);
    //cout<<"size: "<<AdaptersVector.size()<<endl;
    return true;
}
void UOP::Core(){
    for(int n = 0; n < AdaptersVector.size(); n++){
        if(AdaptersVector[n]->IncomePacketAvailable()){
            
        }
    }
}
string UOP::GetVersion(){
    return version;
}
     