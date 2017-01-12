#ifndef __INSTANCE_DATA__
#define __INSTANCE_DATA__

struct InstanceData {
    float4x4 World;
    float4x4 TexTransform;
    uint     MaterialIndex;
    uint   __InstanceDataPad1;
    uint   __InstanceDataPad2;
    uint   __InstanceDataPad3;
};

StructuredBuffer<InstanceData> gInstanceData  : register(REGISTER_SB_INSTANCE_DATA);

#endif
