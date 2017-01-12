#include "consts.shared"
#include "lighting.hlsli"

#define REGISTER_CB_PER_OBJECT    b0
#define REGISTER_CB_PER_PASS      b1

#define REGISTER_SB_INSTANCE_DATA t1

#define SETTINGS_LIGHTING_INSTANCED

#include "InputLayouts\normalTex.hlsli"
#include "ConstantBuffers\PerObject.hlsli"
#include "ConstantBuffers\PerPass.hlsli"
#include "StructuredBuffers\InstanceData.hlsli"

#include "lighting_Vertex.hlsl"
