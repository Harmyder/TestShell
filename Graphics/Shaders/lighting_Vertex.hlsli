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
    float4x4 world = instData.World;
#else
    float4x4 world = gWorld;
#endif

    float4 posW = mul(world, float4(vin.PosL, 1.0f));
    vout.PosW = posW.xyz;
    vout.PosH = mul(gView, posW);
    vout.PosH = mul(gProj, vout.PosH);

    vout.NormalW = mul((float3x3)gWorld, vin.NormalL);
    vout.TexC = vin.TexC;

    return vout;
}
