#include "proto/tuningfork.pb.h"

#include <functional>

namespace mocktuningfork {

void init(const ::com::google::tuningfork::Settings& settings,
          const std::function<void(const ::com::google::tuningfork::FidelityParams&)>& callback);
void set(const ::com::google::tuningfork::Annotation& a);

#define SYS_CPU 0
#define SYS_GPU 1
void tick(int instrumentKey);

}
