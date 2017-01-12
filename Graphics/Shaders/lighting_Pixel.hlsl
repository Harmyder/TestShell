#include "consts.shared"
#include "lighting.hlsli"

#define REGISTER_CB_PER_OBJECT    b0
#define REGISTER_CB_PER_PASS      b1
#define REGISTER_SB_MATERIAL_DATA t0

#define SETTINGS_LIGHTING

#include "InputLayouts\normalTex.hlsli"
#include "ConstantBuffers\PerObject.hlsli"
#include "ConstantBuffers\PerPass.hlsli"
#include "StructuredBuffers\MaterialData.hlsli"

#include "lighting_Pixel.hlsli"
