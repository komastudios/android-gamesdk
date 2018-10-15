#include "proto/mytfannotation.pb.h"
#include "proto/tuningforkannotation.pb.h"
#include <iostream>

using ::logs::proto::tuningfork::Annotation;

void print(const Annotation& t) {
  if(t.loadingstate()==Annotation::LOADING)
    std::cerr << "LOADING";
  else
    std::cerr << "NOT_LOADING";
  std::cerr << " Level=" << t.level();
  std::cerr << " Boss=" << t.GetExtension(bossAlive);
  std::cerr << std::endl;
}
/*
void load() {
  TFAnnotation t;
  t.ParseFromIstream(&std::cin);
  std::cerr << "Loaded ";
  print(t);
}
*/
void save() {
  Annotation t;
  t.set_loadingstate(Annotation::LOADING);
  t.set_level(20);
  t.SetExtension(bossAlive, true);
  t.SerializeToOstream(&std::cout);
  std::cerr << "Saved ";
  print(t);
}

int main(int argc, char * argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if(argc>1)
    load();
  else
    save();
  return 0;
}
