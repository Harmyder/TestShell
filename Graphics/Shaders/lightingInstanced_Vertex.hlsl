#include "consts.shared.h"
#include "lighting.hlsli"

#define REGISTER_CB_PER_OBJECT    b0//REGISTER_LI_CB_PER_OBJECT
#define REGISTER_CB_PER_PASS      b1//REGISTER_LI_CB_PER_PASS
#define REGISTER_SB_INSTANCE_DATA t1//REGISTER_LI_SB_INSTANCE_DATA

#define SETTINGS_LIGHTING_INSTANCED

#include "InputLayouts\normalTex.hlsli"
#include "ConstantBuffers\PerObject.hlsli"
#include "ConstantBuffers\PerPass.hlsli"
#include "StructuredBuffers\InstanceData.hlsli"

#include "lighting_Vertex.hlsl"
