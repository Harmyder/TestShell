[maxvertexcount(4)]
void main(
	point VertexOut position[1], 
	inout TriangleStream<GeoOut> output
)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = gEyePosW - position[0].PosW;
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up, look);
    float halfExtent = .5f * gParticleSize;

    float4 v[4];
    v[0] = float4(position[0].PosW + halfExtent * (right + up), 1.0f);
    v[1] = float4(position[0].PosW + halfExtent * (right - up), 1.0f);
    v[2] = float4(position[0].PosW - halfExtent * (right - up), 1.0f);
    v[3] = float4(position[0].PosW - halfExtent * (right + up), 1.0f);

    float2 texC[4] = {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f) };
    
    GeoOut gout;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(gView, v[i]);
        gout.PosH = mul(gProj, gout.PosH);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;
        gout.TexC = texC[i];
        output.Append(gout);
    }
}