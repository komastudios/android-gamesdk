//
// Created by theoking on 11/14/2019.
//

#include "timing.h"

namespace Timing {

FrameTiming timer;

FrameTiming::FrameTiming() {
  applicationStartTime = std::chrono::high_resolution_clock::now();
}

void FrameTiming::startFrame() {
  if (inActiveFrame)
    stopFrame();

  Frame newSection;
  newSection.startTime = std::chrono::high_resolution_clock::now() - applicationStartTime;
  newSection.number = currentFrame++;
  frames.push_back(newSection);
  inActiveFrame = true;
}

void FrameTiming::startSection(std::string name) {
  if (inActiveSubsection)
    stopSection();

  FrameSection newEvent;
  newEvent.startTime = std::chrono::high_resolution_clock::now() - applicationStartTime;
  newEvent.name = name;
  frames.back().subsectionTimes.push_back(newEvent);
  inActiveSubsection = true;
}

void FrameTiming::stopSection() {
  if (!frames.back().subsectionTimes.empty()) {
    FrameSection *currentSubsection = &frames.back().subsectionTimes.back();
    currentSubsection->duration = std::chrono::high_resolution_clock::now()
        - (currentSubsection->startTime + applicationStartTime);
  }
  inActiveSubsection = false;
}

void FrameTiming::stopFrame() {
  Frame *currentFrame = &frames.back();
  currentFrame->duration =
      std::chrono::high_resolution_clock::now() - (currentFrame->startTime + applicationStartTime);
  inActiveFrame = false;
}

Frame FrameTiming::getLastFrame() {
  return frames.back();
}

float FrameTiming::getFramerate() {
  int framesCount = frames.size() < 100 ? frames.size() : 100;
  float sum = 0;
  for (int x = 0; x < framesCount; x++) {
    sum += frames[frames.size() - x].duration.count();
  }

  return framesCount / (sum * NS_TO_S);
}

}