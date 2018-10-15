// We don't know about the user's annotations, only the base annotations
#include "mocktuningfork.h"
#include "gameengine.h"
#include <iostream>

namespace gameengine {

namespace {
Annotation currentAnnotation;
}

using namespace com::google::tuningfork;

void init(const Settings& s, const std::function<void(const FidelityParams&)>& callback ) {
  mocktuningfork::init(s,callback);
  currentAnnotation.SetExtension(loading_state, LoadingState::LOADING);
  currentAnnotation.SetExtension(level, 0);
}
void init(const std::string& settings, const std::function<void(const std::string&)>& callback ) {
  Settings s;
  s.ParseFromString(settings);
  mocktuningfork::init(s, [&](const FidelityParams& p) { std::string fp_ser; p.SerializeToString(&fp_ser); callback(fp_ser); });
  currentAnnotation.SetExtension(loading_state, LoadingState::LOADING);
  currentAnnotation.SetExtension(level, 0);
}
void set(const std::string& s) {
  Annotation t;
  t.ParseFromString(s);
  currentAnnotation.MergeFrom(t);
}
void set(const Annotation& a) {
  currentAnnotation.MergeFrom(a);
}
void tick() {
  // Some logic for updating the annotation - it usually wouldn't happen every tick
  auto l = currentAnnotation.GetExtension(level);
  currentAnnotation.SetExtension(level, l+1);
  if(l>0)
    currentAnnotation.SetExtension(loading_state, LoadingState::NOT_LOADING);
  mocktuningfork::set(currentAnnotation);
  mocktuningfork::tick(SYS_CPU);
}

}
