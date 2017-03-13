VertexOut main(
    VertexIn vin
#ifdef SETTINGS_LIGHTING_INSTANCED
    , uint instanceId : SV_InstanceID
#endif
)
{
    VertexOut vout;

#ifdef SETTINGS_LIGHTING_INSTANCED
    InstanceData instData = gInstanceData[instanceId];
    vout.MatIndex = instData.MaterialIndex;
    float4x3 iw = instData.World;
    float4x4 world =
        float4x4(
            float4(iw[0][0], iw[1][0], iw[2][0], iw[1][2]),
            float4(iw[3][0], iw[0][1], iw[1][1], iw[2][2]),
            float4(iw[2][1], iw[3][1], iw[0][2], iw[3][2]),
            float4(0.f, 0.f, 0.f, 1.f));

#else
    float4x4 world =
        float4x4(
            float4(gWorld[0][0], gWorld[1][0], gWorld[2][0], gWorld[1][2]),
            float4(gWorld[3][0], gWorld[0][1], gWorld[1][1], gWorld[2][2]),
            float4(gWorld[2][1], gWorld[3][1], gWorld[0][2], gWorld[3][2]),
            float4(0.f, 0.f, 0.f, 1.f));

#endif

    float4 posW = mul(world, float4(vin.PosL, 1.f));
    vout.PosW = posW.xyz;
    vout.PosH = mul(gView, posW);
    vout.PosH = mul(gProj, vout.PosH);

    vout.NormalW = mul((float3x3)gWorld, vin.NormalL);
    vout.TexC = vin.TexC;

    return vout;
}
