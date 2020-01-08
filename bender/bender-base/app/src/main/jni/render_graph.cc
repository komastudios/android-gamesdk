//
// Created by theoking on 1/6/2020.
//

#include "render_graph.h"
#include <math.h>
#include <timing.h>

void RenderGraph::SetCameraPrerotated(bool prerotated) {
  camera_.prerotated = prerotated;
  UpdateCameraPlanes();
}

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
  if (camera_.prerotated) {
    camera_.nearPlaneWidth = camera_.near * tangentAngle;
    camera_.nearPlaneHeight = camera_.near * camera_.aspect_ratio;
    camera_.farPlaneWidth = camera_.far * tangentAngle;
    camera_.farPlaneHeight = camera_.far * camera_.aspect_ratio;
  } else {
    camera_.nearPlaneHeight = camera_.near * tangentAngle;
    camera_.nearPlaneWidth = camera_.near * camera_.aspect_ratio;
    camera_.farPlaneHeight = camera_.far * tangentAngle;
    camera_.farPlaneWidth = camera_.far * camera_.aspect_ratio;
  }
}

std::shared_ptr<Mesh> RenderGraph::GetLastMesh() {
  if (meshes_.size() > 0) {
    return meshes_.back();
  } else {
    return nullptr;
  }
}

void RenderGraph::GetVisibleMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const {
  timing::timer.Time("Get Visible Meshes", timing::OTHER, [this, &meshes]() {
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
    ntl = nearCenter + up * camera_.nearPlaneHeight - right * camera_.nearPlaneWidth;
    ntr = nearCenter + up * camera_.nearPlaneHeight + right * camera_.nearPlaneWidth;
    nbl = nearCenter - up * camera_.nearPlaneHeight - right * camera_.nearPlaneWidth;
    nbr = nearCenter - up * camera_.nearPlaneHeight + right * camera_.nearPlaneWidth;

    // compute the 4 corners of the frustum on the far plane
    ftl = farCenter + up * camera_.farPlaneHeight - right * camera_.farPlaneWidth;
    ftr = farCenter + up * camera_.farPlaneHeight + right * camera_.farPlaneWidth;
    fbl = farCenter - up * camera_.farPlaneHeight - right * camera_.farPlaneWidth;
    fbr = farCenter - up * camera_.farPlaneHeight + right * camera_.farPlaneWidth;

    std::array<Plane, 6> frustumPlanes;
    frustumPlanes[NEARP].normal = -forward;
    frustumPlanes[NEARP].point = nearCenter;
    frustumPlanes[FARP].normal = forward;
    frustumPlanes[FARP].point = farCenter;

    glm::vec3 aux, normal;

    aux = (nearCenter + up * camera_.nearPlaneHeight) - camera_.position;
    normal = glm::cross(glm::normalize(aux), right);
    frustumPlanes[TOP].normal = normal;
    frustumPlanes[TOP].point = nearCenter + up * camera_.nearPlaneHeight;

    aux = (nearCenter - up * camera_.nearPlaneHeight) - camera_.position;
    normal = glm::cross(right, glm::normalize(aux));
    frustumPlanes[BOTTOM].normal = normal;
    frustumPlanes[BOTTOM].point = nearCenter - up * camera_.nearPlaneHeight;

    aux = (nearCenter - right * camera_.nearPlaneWidth) - camera_.position;
    normal = glm::cross(glm::normalize(aux), up);
    frustumPlanes[LEFT].normal = normal;
    frustumPlanes[LEFT].point = nearCenter - right * camera_.nearPlaneWidth;

    aux = (nearCenter + right * camera_.nearPlaneWidth) - camera_.position;
    normal = glm::cross(up, glm::normalize(aux));
    frustumPlanes[RIGHT].normal = normal;
    frustumPlanes[RIGHT].point = nearCenter + right * camera_.nearPlaneWidth;

    for (auto mesh : meshes_) {
      int inside = 0;
      BoundingBox currBox = mesh->GetBoundingBoxWorldSpace();
      for (auto plane : frustumPlanes) {
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