//
// Created by theoking on 11/14/2019.
//

#ifndef BENDER_BASE_UTILS_BENDER_KIT_TIMING_H_
#define BENDER_BASE_UTILS_BENDER_KIT_TIMING_H_

#include <chrono>
#include <vector>
#include <string>
#include <map>

#define NS_TO_S 1/1000000000.0
#define NS_TO_MS 1/1000000.0

namespace Timing {

struct FrameSection {
  std::string name;
  std::chrono::high_resolution_clock::duration startTime, duration;
};

struct Frame {
  int number;
  std::chrono::high_resolution_clock::duration startTime, duration;
  std::vector<FrameSection> subsectionTimes;
};

class FrameTiming {
 public:
  FrameTiming();
  void startFrame();
  void startSection(std::string name);
  void stopSection(std::string name);
  void stopFrame();
  Frame getLastFrame();
  float getFramerate(int numFrames);

  int currentFrame = 0;
  std::map<std::string, int> nameToIndex;
  std::vector<Frame> frames;
  bool inActiveFrame = false;
  std::chrono::high_resolution_clock::time_point applicationStartTime;

};
extern FrameTiming timer;
}

#endif //BENDER_BASE_UTILS_BENDER_KIT_TIMING_H_
