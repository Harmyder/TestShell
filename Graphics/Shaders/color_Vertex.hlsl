#include "cb.hlsli"
#include "color_InputLayout.hlsli"

VertexOut main(VertexIn vin) {
    VertexOut vout;
    float4 posW = mul(gWorld, float4(vin.PosL, 1.0f));
    vout.PosH = mul(gView, posW);
    vout.PosH = mul(gProj, vout.PosH);

    vout.Color = vin.Color;
    return vout;
}