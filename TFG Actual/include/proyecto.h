#include "portaudio.h"
#include "sndfile.h"
#include "Vector2.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <vector>
#include <numeric>
#include <algorithm>

#define MAX_SPEAKERS (2)
#define DEG2RAD  (0.017453292f)

using namespace std;

typedef struct{
  SNDFILE* file = nullptr;
  SF_INFO  info;
  int      buffer_size;
  int      readFrames;
  sf_count_t count;
  PaStream* stream;
  const char* id;

  //Parameters of the playing sound
  float vol;
  float left_pan;
  float right_pan;

  //Physical situation of the sound
  Vector2 position;
  float extent;
  float angle;

  //Extra variables
  bool eof;
  bool hasChannel;
  bool loop;
  int state;
  /*States are
    0: Stopped
    1: To play
    2: Being played
  */
}AudioFile;

typedef struct{
  vector<unique_ptr <AudioFile>> files;
  bool allOver;
}Container;

typedef struct{
  Vector2 leftPos  {-1.0f, 0.0f};
  Vector2 rightPos {1.0f, 0.0f};
  float leftAngle = -180.0f;
  float rightAngle = 180.0f;
}SpeakerPair;

typedef struct{
  Vector2 position {0.0f, 0.0f};
  Vector2 direction {0.0f, 1.0f};
}Listener;

class Sound_Engine{

public:

explicit Sound_Engine() = default;
void initialize();
void loadAudios();
void update(Container *);
void shutdown();
bool checkForCompletion(Container *);
unique_ptr<Container>& getContainer();
unique_ptr<SpeakerPair>& getSpeakers();
unique_ptr<AudioFile>& getAudioById(const char*);
bool getShutdownVar();
void pauseAudio(AudioFile *);
void unpauseAudio(AudioFile *);
void setLoop(AudioFile *, bool);

private:

unique_ptr<AudioFile> loadFile(const char* path);
unique_ptr <Container> container;
unique_ptr<SpeakerPair> speakers;
unique_ptr<Listener> listener;
//PaStream ** streams;
//vector<PaStream *> streams;

array<float, MAX_SPEAKERS> applyVBAP(float, float);
void UpdateSpeakerPanning(array<float, MAX_SPEAKERS>&, float, float, float, float);
float CalculateStereoPanning(const Vector2&, const Vector2&, float, float);
void playFiles(Container *);
void checkAudios(Container *);
void posToAngleExtent(Container *, Listener *);
void errorHandler(PaError);
void setPlaying(AudioFile *);
void display(Container *);

template<class T>
float RootSumSquare(const T&);

bool mode = true;
bool check1 = false;
bool check2 = false;
bool check3 = false;
bool shutdownVar = false;
float var = 0.0f;
bool clock = false;
int rp = 0;
};
