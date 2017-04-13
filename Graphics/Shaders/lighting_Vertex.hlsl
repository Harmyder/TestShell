#include "consts.shared.h"
#include "lighting.hlsli"

#define REGISTER_CB_PER_PASS      b0//REGISTER_L_CB_PER_PASS
#define REGISTER_CB_PER_OBJECT    b1//REGISTER_L_CB_PER_OBJECT

#define SETTINGS_LIGHTING

#include "InputLayouts\normalTex.hlsli"
#include "ConstantBuffers\PerObject.hlsli"
#include "ConstantBuffers\PerPass.hlsli"

#include "lighting_Vertex.hlsli"
