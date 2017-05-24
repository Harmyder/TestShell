#ifndef __CONSTS_SHARED__
    #define __CONSTS_SHARED__

    #define MAX_DIR_LIGHTS_COUNT 3
    #define MAX_PNT_LIGHTS_COUNT 3
    #define MAX_SPT_LIGHTS_COUNT 3

    // Colored
#    define REGISTER_C_CB_PER_OBJECT    b0
#    define REGISTER_C_CB_PER_PASS      b1

    // Lighting
#    define REGISTER_L_CB_PER_OBJECT    b0
#    define REGISTER_L_CB_PER_PASS      b1
#    define REGISTER_L_TB_DIFFUSE_MAP   t0
#    define REGISTER_L_TB_MATERIAL_DATA t1
#    define REGISTER_L_S_SAMPLER_LINEAR s0

    // Lighting instanced
#    define REGISTER_LI_CB_PER_OBJECT    b0
#    define REGISTER_LI_CB_PER_PASS      b1
#    define REGISTER_LI_TB_DIFFUSE_MAP   t0
#    define REGISTER_LI_TB_MATERIAL_DATA t1
#    define REGISTER_LI_TB_INSTANCE_DATA t2
#    define REGISTER_LI_S_SAMPLER_LINEAR s0

    // Particles
#    define REGISTER_P_CB_PARTICLES_META b0
#    define REGISTER_P_CB_PER_PASS       b1
#    define REGISTER_P_TB_DIFFUSE_MAP    t0
#    define REGISTER_P_TB_MATERIAL_DATA  t1
#    define REGISTER_P_S_SAMPLER_LINEAR  s0

#endif // __CONSTS_SHARED__
