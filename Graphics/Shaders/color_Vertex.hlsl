#include "consts.shared.h"

#define REGISTER_CB_PER_OBJECT b0//REGISTER_C_CB_PER_OBJECT
#define REGISTER_CB_PER_PASS   b1//REGISTER_C_CB_PER_OBJECT

#include "InputLayouts\color.hlsli"
#include "ConstantBuffers\PerObject.hlsli"
#include "ConstantBuffers\PerPass.hlsli"

VertexOut main(VertexIn vin) {
    VertexOut vout;
    float4 posW = mul(gWorld, float4(vin.PosL, 1.0f));
    vout.PosH = mul(gView, posW);
    vout.PosH = mul(gProj, vout.PosH);

    vout.Color = vin.Color;
    return vout;
}
