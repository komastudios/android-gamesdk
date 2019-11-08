// This file is the "source of truth" for sets and bindings indices
// and it dictates the descriptors layout for meshes
//
// Shaders and host code use this defines

//////// MATERIAL BINDINGS

#define BINDING_SET_MATERIAL 0
#define FRAGMENT_BINDING_SAMPLER 0

//////// LIGHTS BINDINGS

#define BINDING_SET_LIGHTS 1
#define FRAGMENT_BINDING_LIGHTS 0

//////// MESH BINDINGS

#define BINDING_SET_MESH  2
#define VERTEX_BINDING_MODEL_VIEW_PROJECTION 0
