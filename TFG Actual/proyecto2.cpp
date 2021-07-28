/** @file paex_saw.c
	@ingroup examples_src
	@brief Play a simple (aliasing) sawtooth wave.
	@author Phil Burk  http://www.softsynth.com
*/
/*
 * $Id$
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include <proyecto.h>
#define NUM_SECONDS   (5)
#define SAMPLE_RATE   (48000)
#define CHANNELS      (2)
#define BUFFER_SIZE   (512)

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int playCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData)
{
/*
    // ------------------ EARLY VERSION --------------
    
    // Cast data passed through stream to our structure. 
    float *out = (float*)outputBuffer;
    (void) inputBuffer; // Prevent unused variable warning. 


        SNDFILE * data = (SNDFILE*)userData;
        // Here you need to multiply per 2 for stereo and per 1 for mono
        sf_read_float(data, (float*)outputBuffer, framesPerBuffer*2);
        return paContinue;
        */

    //-------------------- LATER VERSION ---------------
      /// Prevent warnings
  (void)inputBuffer;
  (void)timeInfo;
  (void)statusFlags;
  (void)userData;
  

  /// an AudioFile gets passed as userData
  AudioFile* file = (AudioFile*)userData;
  float*           out  = (float*)outputBuffer;

  sf_seek(file->file, file->readFrames, SF_SEEK_SET);

  auto data   = std::make_unique<float[]>(framesPerBuffer * file->info.channels);
  file->count = sf_read_float(file->file, data.get(), framesPerBuffer * file->info.channels);


  if(file->readFrames + framesPerBuffer > file->info.frames){
      framesPerBuffer = file->info.frames - file->readFrames;
  }

  for (int i = 0; i < framesPerBuffer * file->info.channels && file->eof == false; i++) { 
      if(file->readFrames>=file->info.frames)
        break;

        //En caso de stereo
        if(file->info.channels>1){
            out[i * 2]     = data[i * 2] * file->vol * file->left_pan;
            out[i * 2 + 1] = data[i * 2 + 1] * file->vol * file->right_pan;
        //En caso de mono       
        }else{
            out[i * 2]       = data[i] * file->vol * file->left_pan;
            out[i * 2 + 1]   = data[i] * file->vol * file->right_pan;
        }

        
      /*if(file->info.channels>1)
        *out++ = data[i * 2 + 1] * file->vol * file->right_pan;*/
      //Otra forma de ponerlo seria
      //out[i * 2] = ...
      //if()
      //out[i * 2 + 1]      
  }

   // cout<<"Frames totales: "<<file->readFrames<<" vs "<<file->info.frames<<endl;
  if(file->readFrames<file->info.frames){
    file->readFrames += framesPerBuffer;
    file->state = 2;
    return paContinue;
  }
  else{
      if(!file->loop){
        file->eof = true;
        file->readFrames = 0;
        file->state = 0;
        return paComplete;
      }
      else{
        file->readFrames = 0;
        file->state = 1;
        //file->hasChannel = false;
        return paContinue;
      }
    
  }
}


/*******************************************************************/

void Sound_Engine::initialize(){
    /* Initialize library before making any other calls. */
    PaError err;
    err = Pa_Initialize();
    if( err != paNoError ) errorHandler(err);

    printf("PortAudio Test: output sound file.\n");
    speakers = make_unique<SpeakerPair>();
    container = make_unique<Container>();
    listener  = make_unique<Listener>();

}

void Sound_Engine::loadAudios(){

    //ESTO EN LA VERSION DEFINITIVA, DEBERIA DE SIMPLEMENTE RECIBIR
    //EL CONTAINER Y EN FUNCION DE SU TAMAÑO CREAR LOS STREAMS

    container.get()->files.push_back(loadFile("AudioFiles/sound.wav"));
    setPlaying(container.get()->files[0].get());
    /*container.get()->files.push_back(loadFile("AudioFiles/yeet.wav"));
    setPlaying(container.get()->files[1].get());
    container.get()->files.push_back(loadFile("AudioFiles/meme.wav"));
    setPlaying(container.get()->files[2].get());*/
    //container.get()->files[1]->state = 1;



    /*streams = new PaStream* [container.get()->files.size()];
    //Make all the streams null for the first time
    for (int i = 0; i<container.get()->files.size(); i++)
        streams[i] = nullptr;*/

}

void Sound_Engine::update(Container *cont){

    //AudioFile *file;
    //1st value indicates the angle from the vector [0, 1], variable between -180 
    //and 180. 2nd value indicates how close from the listener it is, by how 
    //much it occupies on the speaker range, from 0 non audible to 360, on the listener.
    
    /*For Pan Demo*/
    if(container.get()->files[0]->angle <= 0){
        container.get()->files[0]->angle -= 0.001;
        if(container.get()->files[0]->angle<=-180){
            container.get()->files[0]->angle = 180;
        }
    }else if (container.get()->files[0]->angle > 0){
        container.get()->files[0]->angle -= 0.001;
    }

    cout<<"Valor de la direccion: "<<container.get()->files[0]->angle<<endl;
    cout<<"Ganancia en canal Izquierdo: "<<container.get()->files[0]->left_pan<<endl;
    cout<<"Ganancia en canal Derecho: "<<container.get()->files[0]->right_pan<<endl;


    //posToAngleExtent(cont, listener.get());


    for(int pos=0; pos<container->files.size(); pos++){
        if(container->files[pos]->state == 2){
            container.get()->files[pos]->right_pan = applyVBAP(container.get()->files[pos]->angle, container.get()->files[pos]->extent)[0];
            container.get()->files[pos]->left_pan  = applyVBAP(container.get()->files[pos]->angle, container.get()->files[pos]->extent)[1];
        }
    }
    playFiles(cont);
     /*if(check1 == false){
        if(container->files[0]->eof == true ){
            container.get()->files.push_back(loadFile("AudioFiles/yeet.wav"));
            setPlaying(container->files.back().get());
            container.get()->files.push_back(loadFile("AudioFiles/meme.wav"));
            setPlaying(container->files.back().get());
            check1 = true;
            
        }
    }*/

    checkAudios(cont);
    //display(cont);
    
    
    //Por si se quiere que el programa cierre cuando se han acabado los audios
    shutdownVar = checkForCompletion(cont);


}

void Sound_Engine::shutdown(){

    PaError err;

    //Does this in case some stream is still open
    for(int pos=0; pos<container->files.size(); pos++){
        if(container->files[pos]->stream){
            cout<<"Paramos el "<<pos<<endl;
            err = Pa_StopStream( container->files[pos]->stream );
            if( err != paNoError ) errorHandler(err);
            cout<<"Cerramos el "<<pos<<endl;

            err = Pa_CloseStream( container->files[pos]->stream );
            if( err != paNoError ) errorHandler(err);
            cout<<"Todo bien"<<endl;
            container->files[pos]->hasChannel = false;
            container->files[pos]->stream = nullptr;
        }
    }

    for(int pos=0; pos<container->files.size(); pos++){
            container->files[pos]->stream = nullptr;
    }

    container->files.clear();

    err = Pa_Terminate();
    if( err != paNoError ) errorHandler(err);
    printf("PortAudio terminated succesfully\n");

}

unique_ptr<AudioFile> Sound_Engine::loadFile(const char* path){

    unique_ptr<AudioFile> file = make_unique<AudioFile>();
    //Fills the SF_INFO with the data of the audio
    ::memset(&file->info, 0, sizeof(file->info));
     file->file = sf_open(path, SFM_READ, &file->info);

    //Se le debería dar mas
    file->buffer_size = 1;
    file->readFrames = 0;
    file->count = 1;
    file->vol = 1.0f;
    file->left_pan = 1.0f;
    file->right_pan = 1.0f;
    file->eof = false;
    file->hasChannel = false;
    file->loop = false;
    file->state = 0;
    file->position.x = 0.0f;
    file->position.y = 1.0f;
    file->stream = nullptr;
    file->id = path;
    cout<<"Opening "<<file->id<<endl;
    printf("%d frames %d samplerate %d channels\n", (int)file->info.frames,
	file->info.samplerate, file->info.channels);

     return file;
}

void Sound_Engine::posToAngleExtent(Container* cont, Listener* listener){
    int a = 0;
    for(int pos=0; pos<cont->files.size(); pos++){
        Vector2 sound2listener = {cont->files[pos]->position.x - listener->position.x, cont->files[pos]->position.y -  listener->position.y};


        cont->files[pos]->angle = 
            acos( (sound2listener.Dot(listener->direction)) / (sqrt( pow(sound2listener.x, 2) + pow(sound2listener.y, 2) ) * sqrt( pow(listener->direction.x, 2) + pow(listener->direction.y, 2)))) / DEG2RAD;

        if(sound2listener.x < listener->direction.x)
            cont->files[pos]->angle = -cont->files[pos]->angle;
            //Sacar el extent segun el modulo del vector entre posicion del listener y posicion del audio

        float module = sqrt(pow(sound2listener.x, 2) + pow(sound2listener.y, 2));
        //cout<<module<<endl;
        cont->files[pos]->extent = 100 * (3.6 - log(module + 1));
        if(cont->files[pos]->extent <= 0)
            cont->files[pos]->extent = 0.1f;
        a++;
    }
}

array<float, MAX_SPEAKERS> Sound_Engine::applyVBAP(float fSourceDirection, float fSourceExtent){
    //Cositas libro
    array<float, MAX_SPEAKERS> pOutPut;
    pOutPut.fill(0.0f);

    if(mode == false){
        pOutPut[0] = 1.0f;
        pOutPut[1] = 1.0f;
        return pOutPut;
    }
    float fHalfExtent = fSourceExtent / 2.0f;
    float fMinAngle = fSourceDirection - fHalfExtent;
    float fMaxAngle = fSourceDirection + fHalfExtent;

    //cout<<"Angulo minimo: "<<fMinAngle<<endl;
    //cout<<"Angulo maximo: "<<fMaxAngle<<endl;


    //Ahora vendrian las iteraciones entre altavoces, pero por ahora
    //lo vamos a intentar hacer sobre los auriculares normales
    if(speakers->rightAngle >= 0.0f || speakers->leftAngle <= 0.0f)
        UpdateSpeakerPanning(pOutPut, fMinAngle, fMaxAngle, speakers->leftAngle, speakers->rightAngle);
    else{
        UpdateSpeakerPanning(pOutPut, fMinAngle, fMaxAngle, speakers->leftAngle, speakers->rightAngle + 2.0f * 3.1415f);
        UpdateSpeakerPanning(pOutPut, fMinAngle, fMaxAngle, speakers->leftAngle - 2.0f * 3.1415f, speakers->rightAngle);
    }

    /*float fRss = RootSumSquare(pOutPut);
    if(fRss != 0.0f){
        for_each(pOutPut.begin(), pOutPut.end(), [=](float& v){ v /= fRss; });
    }*/

    //cout<<"Valor del array (derecha?) "<<pOutPut[0]<<endl;
    //cout<<"Valor del arry (izquierda?) "<<pOutPut[1]<<endl;
    return pOutPut;

}

void Sound_Engine::UpdateSpeakerPanning(array<float, MAX_SPEAKERS>& pOutPut, float fMinAngle, float fMaxAngle, float fLeftAngle, float fThisAngle){
    
    if((fThisAngle < fMinAngle) || (fLeftAngle > fMaxAngle))
        return;

    float fFromAngle = max(fLeftAngle, fMinAngle);
    float fToAngle = min(fThisAngle, fMaxAngle);

    //float fFromAngle = fMinAngle;
    //float fToAngle = fMaxAngle;

    //cout<<"Angulos FromAngle y ToAngle: "<<fFromAngle<<" "<<fToAngle<<endl;
    if(fFromAngle > fToAngle)
        swap(fFromAngle, fToAngle);

    float fDirection = fFromAngle + (fToAngle - fFromAngle) / 2.0f;
    //float fDirection = var;
    //cout<<"Direccion: "<<fDirection<<endl;
    float fGain = (fToAngle - fFromAngle) / (fThisAngle - fLeftAngle);
    //float fGain = 0.33f;
    //cout<<"Gain: "<<fGain<<endl;

    if(fGain == 0.0f && fMinAngle == fMaxAngle)
        fGain = 1.0f;

    //if para ver el angulo entre los altavoces, en este caso va a ser 
    //siempre igual
    Vector2 vContributions {0.0f, 0.0f};
    Vector2 vScaledSource {sin(fDirection * DEG2RAD), cos(fDirection * DEG2RAD)};


    float fSpeakerDistance = getSpeakers().get()->leftPos.Distance(getSpeakers().get()->rightPos);
    
    //cout<<"Distancia entre altavoces: "<<fSpeakerDistance<<endl;

    //cout<<"El derecho"<<endl;
    vContributions.x = CalculateStereoPanning(getSpeakers().get()->rightPos, vScaledSource, fSpeakerDistance, fGain);
    //cout<<"El izquierdo"<<endl;    
    vContributions.y = CalculateStereoPanning(getSpeakers().get()->leftPos, vScaledSource, fSpeakerDistance, fGain);

    auto tContributions = {vContributions.x, vContributions.y};
    float fMultiplier = 1.0f;

    /*float fRss = RootSumSquare(tContributions);
    if(fRss != 0.0f)
    {
        fMultiplier = fGain / fRss;
    }*/
    pOutPut[0] += vContributions.x * fMultiplier;
    pOutPut[1] += vContributions.y * fMultiplier;
}

float Sound_Engine::CalculateStereoPanning(const Vector2& speakerPos, const Vector2& vScaledSource, float fSpeakerDistance, float fGain){
    float fContribution = 
        1.0f - (speakerPos.Distance(speakerPos +  speakerPos * speakerPos.Dot(vScaledSource - speakerPos) * speakerPos) / fSpeakerDistance);
        //cout<<"Los calculos son: 1.0f - Distancia de ("<<speakerPos.x<<" + "<<speakerPos.x<<" * Dot entre ("<<vScaledSource.x<<" y "<<speakerPos.x<<")) / "<<fSpeakerDistance<<endl;
        //cout<<"Stereo panning saca: "<<fContribution<<" * "<<fGain<<endl;
        return fContribution * fGain;
        
}


template<class T>
float Sound_Engine::RootSumSquare(const T& t){
    float fSumOfSquares = std::accumulate(begin(t), end(t), 0.0f,
        [](float fSum, float fEntry)
        {return fSum + fEntry * fEntry;});
    return sqrt(fSumOfSquares);
}

void Sound_Engine::playFiles(Container* cont){
    //PaStream * stream[cont->files.size()];
    PaError err;

    // Opens an audio I/O stream. 
    for(int pos=0; pos<cont->files.size(); pos++){
        if(cont->files[pos]->state == 1 && !cont->files[pos]->hasChannel){
            err = Pa_OpenDefaultStream( &cont->files[pos]->stream,
                                        0,          /* no input channels */
                                        2,          /* stereo output */
                                        paFloat32,  /* 32 bit floating point output */
                                        cont->files[pos]->info.samplerate,
                                        cont->files[pos]->buffer_size,        /* frames per buffer */
                                        playCallback,
                                        cont->files[pos].get());
            if( err != paNoError ) errorHandler(err);

            err = Pa_StartStream( cont->files[pos]->stream );
            if( err != paNoError ) errorHandler(err);
            cont->files[pos]->hasChannel = true;
            cont->files[pos]->state = 2;
            cout<<"Damos channel"<<endl;
        }

        if(cont->files[pos]->state == 1 && cont->files[pos]->hasChannel){
            /*loop case*/
            err = Pa_StartStream( cont->files[pos]->stream );
            rp++;
            if(cont->files[pos]->stream)
                cout<<"Bien"<<endl;

        }
    }
}

void Sound_Engine::setPlaying(AudioFile * audio){
    if(audio->state == 0){
        audio->state = 1;
        audio->eof = false;
        audio->hasChannel = false;
    }
}

void Sound_Engine::unpauseAudio(AudioFile * audio){
    PaError err;
    if(audio->state == 0 && audio->hasChannel){
        audio->state = 2;
        err = Pa_StartStream(audio->stream);
    }

}

void Sound_Engine::pauseAudio(AudioFile * audio){
    PaError err;
    if(audio->state == 2 && audio->hasChannel){
        audio->state = 0;
        err = Pa_StopStream(audio->stream);
    }
}

void Sound_Engine::setLoop(AudioFile * audio, bool var){
    audio->loop = var;
}

void Sound_Engine::checkAudios(Container* cont){

    PaError err;

    string nombre;

    for(int pos=0; pos<cont->files.size(); pos++){
        if(cont->files[pos]->eof == true && cont->files[pos]->hasChannel){
                nombre = cont->files[pos]->id;
                cout<<"Paramos el "<<pos<<endl;
                err = Pa_StopStream( cont->files[pos]->stream );
                if( err != paNoError ) errorHandler(err);
                cout<<"Cerramos el "<<pos<<endl;
                err = Pa_CloseStream( cont->files[pos]->stream );
                if( err != paNoError ) errorHandler(err);
                cont->files[pos]->hasChannel = false;
                cont->files[pos]->stream = nullptr;
                cont->files.erase(cont->files.begin() + pos);
                /*streams[pos] = nullptr;
                streams.erase(streams.begin() + pos);
                delete streams[pos];*/
                cout<<"Todo bien para el fichero "<<nombre<<endl;

        }
    }
}

bool Sound_Engine::checkForCompletion(Container* cont){
    bool var = true;
    for(int pos=0; pos<container->files.size(); pos++){
        if(container->files[pos]->eof == false)
            var = false;
    }
    return var;
}

unique_ptr<Container>& Sound_Engine::getContainer(){
    return container;
}

unique_ptr<SpeakerPair>& Sound_Engine::getSpeakers(){
    return speakers;
}

bool Sound_Engine::getShutdownVar(){
    return shutdownVar;
}

void Sound_Engine::display(Container* cont){

    for(int pos=0; pos<container->files.size(); pos++){
        if (container->files.empty()){
        cout<<"No hay na ya "<<endl;
        break;
    }
        cout<<"Elemento "<<pos<<" siendo "<<container->files[pos]->id<<" con State "<<container->files[pos]->state<<endl;
            if(container->files[pos]->stream)
                cout<<"Tiene stream asignado"<<endl;
            else
                cout<<"No tiene stream asignado"<<endl;
    }
    cout<<"Hay "<<container->files.size()<<" elementos"<<endl;
}

unique_ptr<AudioFile>& Sound_Engine::getAudioById(const char* path){
    for(int pos = 0; pos<container->files.size(); pos++)
    {
        if(strcmp(container->files[pos]->id, path) == 0)
            return container->files[pos];
    }

}

void Sound_Engine::errorHandler(PaError error){
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", error );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( error ) );
}

