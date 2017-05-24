#include "consts.shared.h"
#include "lighting.hlsli"

#define REGISTER_CB_PARTICLES_META b0//REGISTER_P_CB_PARTICLES_META
#define REGISTER_CB_PER_PASS       b1//REGISTER_P_CB_PER_PASS

#define SETTINGS_PARTICLES

#include "InputLayouts\particle.hlsli"
#include "ConstantBuffers\ParticlesMeta.hlsli"
#include "ConstantBuffers\PerPass.hlsli"

#include "particles_Vertex.hlsli"
