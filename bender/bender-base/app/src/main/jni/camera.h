//
// Created by theoking on 1/27/2020.
//

#ifndef BENDER_BASE_APP_SRC_MAIN_JNI_CAMERA_H_
#define BENDER_BASE_APP_SRC_MAIN_JNI_CAMERA_H_

#include <array>
#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>

const float kPlaneWidthTolerance = 1.2f;

struct Plane {
  glm::vec3 point;
  glm::vec3 normal;
};

enum FrustumPlanes {
  TOP = 0, BOTTOM, LEFT,
  RIGHT, NEARP, FARP
};

struct Camera {
#ifndef GDC_DEMO
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  float far = 100.0f;
#else
  glm::vec3 position = glm::vec3(0.0f, 100.0f, -1500.0f);
  float far = 3000.0f;
#endif
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  float aspect_ratio;
  float fov;
  float near = .1f;
  float near_plane_width, near_plane_height, far_plane_width, far_plane_height;
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 prerotation;

  std::array<Plane, 6> GenerateFrustumPlanes() const;
  void UpdateCameraPlanes();
};

#endif //BENDER_BASE_APP_SRC_MAIN_JNI_CAMERA_H_
