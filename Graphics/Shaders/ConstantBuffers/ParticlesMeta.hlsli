#ifndef __PARTICLES_META__
#define __PARTICLES_META__

cbuffer cbParticlesMeta : register(REGISTER_CB_PARTICLES_META)
{
    float4x3 gWorld;
    float    gParticleSize;
    uint     gMaterialIndex;
    uint __ParticlesMetaPad1;
    uint __ParticlesMetaPad2;
};

#endif
