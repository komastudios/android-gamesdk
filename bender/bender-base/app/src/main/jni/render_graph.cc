//
// Created by theoking on 12/9/2019.
//

#include "render_graph.h"

void RenderGraph::onFocus() {


  for (auto &mesh : meshes){
    mesh.onFocus();
  }
}