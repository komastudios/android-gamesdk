//
// Created by theoking on 1/6/2020.
//

#include "render_graph.h"
#include <math.h>
#include <timing.h>

void RenderGraph::SetCameraFOV(float fov) {
  camera_.fov = fov;
  camera_.UpdateCameraPlanes();
}

void RenderGraph::SetCameraAspectRatio(float aspect_ratio) {
  camera_.aspect_ratio = aspect_ratio;
  camera_.UpdateCameraPlanes();
}

std::shared_ptr<Mesh> RenderGraph::GetLastMesh() const {
  if (meshes_.size() > 0) {
    return meshes_.back();
  } else {
    return nullptr;
  }
}

void RenderGraph::GetVisibleMeshes(std::vector<std::shared_ptr<Mesh>> &meshes) const {
  timing::timer.Time("Get Visible Meshes", timing::OTHER, [this, &meshes]() {
    meshes.clear();

    std::array<Plane, 6> frustum_planes = camera_.GenerateFrustumPlanes();

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

//    std::sort(meshes.begin(),
//              meshes.end(),
//              [this, meshes](std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b) {
//                BoundingBox meshA = a->GetBoundingBoxWorldSpace();
//                BoundingBox meshB = b->GetBoundingBoxWorldSpace();
//                float distanceA = glm::length(meshA.center - camera_.position);
//                float distanceB = glm::length(meshB.center - camera_.position);
//                return distanceA < distanceB;
//              });
  });
}