#include "cb.hlsli"

VertexOut main(VertexIn vin)
{
    VertexOut vout;

    // Transform to homogeneous clip space.
    float4 posW = mul(gWorld, float4(vin.PosL, 1.0f));
    posW = mul(gView, posW);
    posW = mul(gProj, posW);
    vout.PosH = posW;

    // Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    return vout;
}
