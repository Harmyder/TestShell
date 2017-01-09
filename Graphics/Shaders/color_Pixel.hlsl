#include "cb.hlsli"
#include "color_InputLayout.hlsli"

float4 main(VertexOut pin) : SV_Target
{
    return pin.Color;
}
