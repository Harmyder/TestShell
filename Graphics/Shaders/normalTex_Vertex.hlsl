#include "cb.hlsli"
#include "normalTex_InputLayout.hlsli"

VertexOut main(VertexIn vin)
{
    VertexOut vout;

    // Transform to homogeneous clip space.
    float4 posW = mul(gWorld, float4(vin.PosL, 1.0f));
    vout.PosW = posW.xyz;
    vout.PosH = mul(gView, posW);
    vout.PosH = mul(gProj, vout.PosH);

    vout.NormalW = mul((float3x3)gWorld, vin.NormalL);
    vout.TexC = vin.TexC;

    return vout;
}
