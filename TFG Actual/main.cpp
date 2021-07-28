#include <proyecto.h>

int main(void){
    cout<<"WELCOME"<<endl;
    unique_ptr<Sound_Engine> sound_engine;
    sound_engine = make_unique<Sound_Engine>();
    cout<<"Entra initialize"<<endl;
    sound_engine->initialize();
    sound_engine->loadAudios();
    cout<<"Entra update"<<endl;
    while(!sound_engine->getShutdownVar()){
        sound_engine->update(sound_engine->getContainer().get());
        
    }
    cout<<"Entra shutdown"<<endl;
    sound_engine->shutdown();
}