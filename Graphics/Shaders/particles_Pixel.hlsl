#include "consts.shared.h"
#include "lighting.hlsli"

#define REGISTER_CB_PARTICLES_META b0
#define REGISTER_CB_PER_PASS       b1
#define REGISTER_TEX_DIFFUSE_MAP   t0
#define REGISTER_SB_MATERIAL_DATA  t1
#define REGISTER_S_SAMPLER_LINEAR  s0

#define SETTINGS_PARTICLES

#include "InputLayouts\particle.hlsli"
#include "ConstantBuffers\ParticlesMeta.hlsli"
#include "ConstantBuffers\PerPass.hlsli"
#include "StructuredBuffers\MaterialData.hlsli"

#include "lighting_Pixel.hlsli"