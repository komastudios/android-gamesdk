//
// Created by theoking on 1/6/2020.
//

#include "render_graph.h"
#include <math.h>
#include <timing.h>

void RenderGraph::SetCameraFOV(float fov) {
  camera_.fov = fov;
  UpdateCameraPlanes();
}

void RenderGraph::SetCameraAspectRatio(float aspect_ratio) {
  camera_.aspect_ratio = aspect_ratio;
  UpdateCameraPlanes();
}

void RenderGraph::UpdateCameraPlanes() {
  float tangentAngle = tan(camera_.fov * .5);
  camera_.near_plane_height = camera_.near * tangentAngle;
  camera_.near_plane_width = camera_.near * camera_.aspect_ratio;
  camera_.far_plane_height = camera_.far * tangentAngle;
  camera_.far_plane_width = camera_.far * camera_.aspect_ratio;
}

std::shared_ptr<Mesh> RenderGraph::GetLastMesh() const {
  if (meshes_.size() > 0) {
    return meshes_.back();
  } else {
    return nullptr;
  }
}

std::array<Plane, 6> RenderGraph::GenerateFrustumPlanes() const{
  glm::vec3 nearCenter, farCenter, right, up, forward;
  glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;

  // This forward vector is opposite the view direction
  forward = camera_.rotation * glm::vec3(0, 0, 1);
  right = camera_.rotation * glm::vec3(1, 0, 0);
  up = camera_.rotation * glm::vec3(0, 1, 0);

  // compute the centers of the near and far planes
  nearCenter = camera_.position - forward * camera_.near;
  farCenter = camera_.position - forward * camera_.far;

  // compute the 4 corners of the frustum on the near plane
  ntl = nearCenter + up * camera_.near_plane_height - right * camera_.near_plane_width;
  ntr = nearCenter + up * camera_.near_plane_height + right * camera_.near_plane_width;
  nbl = nearCenter - up * camera_.near_plane_height - right * camera_.near_plane_width;
  nbr = nearCenter - up * camera_.near_plane_height + right * camera_.near_plane_width;

  // compute the 4 corners of the frustum on the far plane
  ftl = farCenter + up * camera_.far_plane_height - right * camera_.far_plane_width;
  ftr = farCenter + up * camera_.far_plane_height + right * camera_.far_plane_width;
  fbl = farCenter - up * camera_.far_plane_height - right * camera_.far_plane_width;
  fbr = farCenter - up * camera_.far_plane_height + right * camera_.far_plane_width;

  std::array<Plane, 6> frustum_planes;
  frustum_planes[NEARP].normal = -forward;
  frustum_planes[NEARP].point = nearCenter;
  frustum_planes[FARP].normal = forward;
  frustum_planes[FARP].point = farCenter;

  glm::vec3 aux, normal;

  aux = (nearCenter + up * camera_.near_plane_height) - camera_.position;
  normal = glm::cross(glm::normalize(aux), right);
  frustum_planes[TOP].normal = normal;
  frustum_planes[TOP].point = nearCenter + up * camera_.near_plane_height;

  aux = (nearCenter - up * camera_.near_plane_height) - camera_.position;
  normal = glm::cross(right, glm::normalize(aux));
  frustum_planes[BOTTOM].normal = normal;
  frustum_planes[BOTTOM].point = nearCenter - up * camera_.near_plane_height;

  aux = (nearCenter - right * camera_.near_plane_width) - camera_.position;
  normal = glm::cross(glm::normalize(aux), up);
  frustum_planes[LEFT].normal = normal;
  frustum_planes[LEFT].point = nearCenter - right * camera_.near_plane_width;

  aux = (nearCenter + right * camera_.near_plane_width) - camera_.position;
  normal = glm::cross(up, glm::normalize(aux));
  frustum_planes[RIGHT].normal = normal;
  frustum_planes[RIGHT].point = nearCenter + right * camera_.near_plane_width;

  return frustum_planes;
}

void RenderGraph::GetVisibleMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const {
  timing::timer.Time("Get Visible Meshes", timing::OTHER, [this, &meshes]() {
    meshes.clear();

    std::array<Plane, 6> frustum_planes = GenerateFrustumPlanes();

    for (auto mesh : meshes_) {
      int inside = 0;
      BoundingBox currBox = mesh->GetBoundingBoxWorldSpace();
      for (auto plane : frustum_planes) {
        glm::vec3 normal = plane.normal;
        glm::vec3 positive = currBox.min;
        if (normal.x >= 0)
          positive.x = currBox.max.x;
        if (normal.y >= 0)
          positive.y = currBox.max.y;
        if (normal.z >= 0)
          positive.z = currBox.max.z;

        glm::vec3 negative = currBox.max;
        if (normal.x >= 0)
          negative.x = currBox.min.x;
        if (normal.y >= 0)
          negative.y = currBox.min.y;
        if (normal.z >= 0)
          negative.z = currBox.min.z;

        if (glm::dot(normal, positive - plane.point) < 0) {
          break;
        }
        inside++;
      }
      if (inside == 6) {
        meshes.push_back(mesh);
      }
    }

    std::sort(meshes.begin(),
              meshes.end(),
              [this, meshes](std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b) {
                float distanceA = glm::length(a->GetPosition() - camera_.position);
                float distanceB = glm::length(b->GetPosition() - camera_.position);
                return distanceA < distanceB;
              });
  });
}