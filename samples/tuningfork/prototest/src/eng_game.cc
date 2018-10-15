#include "proto/eng_tuningfork_extensions.pb.h"
#include <iostream>
#include "mocktuningfork.h"
#include "gameengine.h"

using ::com::google::tuningfork::Annotation;
using ::com::google::tuningfork::Settings;
using ::com::google::tuningfork::FidelityParams;

// Get the game engine to fill in loading state and level
void setUsingGameEngine() {
  Annotation t;
  t.SetExtension(bossAlive, true);
  gameengine::set(t);
}

// This is e.g. a Unity C# script setting an annotation
void setUsingGameEngineAsIfAnotherLanguage() {
  Annotation t;
  t.SetExtension(bossAlive, true);
  std::string s;
  t.SerializeToString(&s);
  gameengine::set(s);
}

FidelityParams fparams;

void fidelityParamsCallback(const FidelityParams& p) {
  fparams = p;
  std::cout << "Game engine got fidelity parameters from TF: " << fparams.DebugString() << std::endl;
}

void runWithGameEngine(size_t iters = 1, bool asIfAnotherLanguage = false) {
  Settings settings;
  Annotation ann;
  bool bAlive = false;
  if(asIfAnotherLanguage)
    gameengine::init(settings, fidelityParamsCallback);
  else {
    std::string settings_ser;
    settings.SerializeToString(&settings_ser);
    gameengine::init(settings_ser, [&](const std::string& fp_ser) { FidelityParams fp; fp.ParseFromString(fp_ser); fidelityParamsCallback(fp);});
  }

  for(size_t i=0; i<iters; ++i) {
    bAlive = !bAlive;
    ann.SetExtension(bossAlive, bAlive);
    if(asIfAnotherLanguage) {
      std::string str;
      ann.SerializeToString(&str);
      gameengine::set(str);
    }
    else
      gameengine::set(ann);
    gameengine::tick();
  }
}

int main(int argc, char * argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::cout << "** With Game engine ** \n";
  runWithGameEngine(3);

  std::cout << "** With Game engine (we're not C++)** \n";
  runWithGameEngine(3, true);

  return 0;
}
