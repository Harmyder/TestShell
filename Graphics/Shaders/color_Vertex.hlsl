#include "consts.shared.h"

#define REGISTER_CB_PER_PASS   b0//REGISTER_C_CB_PER_OBJECT
#define REGISTER_CB_PER_OBJECT b1//REGISTER_C_CB_PER_OBJECT

#include "InputLayouts\color.hlsli"
#include "ConstantBuffers\PerObject.hlsli"
#include "ConstantBuffers\PerPass.hlsli"

VertexOut main(VertexIn vin) {
    float4x4 world =
        float4x4(
            float4(gWorld[0][0], gWorld[1][0], gWorld[2][0], gWorld[1][2]),
            float4(gWorld[3][0], gWorld[0][1], gWorld[1][1], gWorld[2][2]),
            float4(gWorld[2][1], gWorld[3][1], gWorld[0][2], gWorld[3][2]),
            float4(0.f, 0.f, 0.f, 1.f));

    VertexOut vout;
    float4 posW = mul(world, float4(vin.PosL, 1.0f));
    vout.PosH = mul(gView, posW);
    vout.PosH = mul(gProj, vout.PosH);

    vout.Color = vin.Color;
    return vout;
}
