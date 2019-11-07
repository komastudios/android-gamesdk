// This file is the "source of truth" for sets and bindings indices
// and it dictates the descriptors layout for meshes
//
// Shaders and host code use this defines

//////// MATERIAL BINDINGS

#define BINDING_SET_MATERIAL 0
#define FRAGMENT_BINDING_SAMPLER 1

//////// LIGHTS BINDINGS

// TODO: change the lights set index once the lights set is extracted
#define BINDING_SET_LIGHTS BINDING_SET_MATERIAL
#define FRAGMENT_BINDING_LIGHTS 2

//////// MESH BINDINGS

#define BINDING_SET_MESH  1
#define VERTEX_BINDING_MODEL_VIEW_PROJECTION 0
