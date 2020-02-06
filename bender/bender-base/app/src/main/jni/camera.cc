//
// Created by theoking on 1/28/2020.
//

#include "camera.h"

void Camera::UpdateCameraPlanes() {
  float tangentAngle = tan(fov * .5);
  near_plane_height = near * tangentAngle;
  near_plane_width = near * aspect_ratio;
  far_plane_height = far * tangentAngle;
  far_plane_width = far * aspect_ratio;
}

std::array<Plane, 6> Camera::GenerateFrustumPlanes() const{
  glm::vec3 nearCenter, farCenter, right, up, forward;
  glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;

  // This forward vector is opposite the view direction
  forward = rotation * glm::vec3(0, 0, 1);
  right = rotation * glm::vec3(1, 0, 0);
  up = rotation * glm::vec3(0, 1, 0);

  // compute the centers of the near and far planes
  nearCenter = position - forward * near;
  farCenter = position - forward * far;

  // compute the 4 corners of the frustum on the near plane
  ntl = nearCenter + up * near_plane_height - right * near_plane_width;
  ntr = nearCenter + up * near_plane_height + right * near_plane_width;
  nbl = nearCenter - up * near_plane_height - right * near_plane_width;
  nbr = nearCenter - up * near_plane_height + right * near_plane_width;

  // compute the 4 corners of the frustum on the far plane
  ftl = farCenter + up * far_plane_height - right * far_plane_width;
  ftr = farCenter + up * far_plane_height + right * far_plane_width;
  fbl = farCenter - up * far_plane_height - right * far_plane_width;
  fbr = farCenter - up * far_plane_height + right * far_plane_width;

  std::array<Plane, 6> frustum_planes;
  frustum_planes[NEARP].normal = -forward;
  frustum_planes[NEARP].point = nearCenter;
  frustum_planes[FARP].normal = forward;
  frustum_planes[FARP].point = farCenter;

  glm::vec3 aux, normal;

  aux = (nearCenter + up * near_plane_height) - position;
  normal = glm::cross(glm::normalize(aux), right);
  frustum_planes[TOP].normal = normal;
  frustum_planes[TOP].point = nearCenter + up * near_plane_height;

  aux = (nearCenter - up * near_plane_height) - position;
  normal = glm::cross(right, glm::normalize(aux));
  frustum_planes[BOTTOM].normal = normal;
  frustum_planes[BOTTOM].point = nearCenter - up * near_plane_height;

  aux = (nearCenter - right * near_plane_width) - position;
  normal = glm::cross(glm::normalize(aux), up);
  frustum_planes[LEFT].normal = normal;
  frustum_planes[LEFT].point = nearCenter - right * near_plane_width;

  aux = (nearCenter + right * near_plane_width) - position;
  normal = glm::cross(up, glm::normalize(aux));
  frustum_planes[RIGHT].normal = normal;
  frustum_planes[RIGHT].point = nearCenter + right * near_plane_width;

  return frustum_planes;
}