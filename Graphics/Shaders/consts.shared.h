#ifndef __CONSTS_SHARED__
    #define __CONSTS_SHARED__

    #define MAX_DIR_LIGHTS_COUNT 3
    #define MAX_PNT_LIGHTS_COUNT 3
    #define MAX_SPT_LIGHTS_COUNT 3

    #define REGISTER_C_CB_PER_OBJECT    b0
    #define REGISTER_C_CB_PER_PASS      b1

    #define REGISTER_L_CB_PER_OBJECT    b0
    #define REGISTER_L_CB_PER_PASS      b1
    #define REGISTER_L_SB_MATERIAL_DATA t0

    #define REGISTER_LI_CB_PER_OBJECT    b0
    #define REGISTER_LI_CB_PER_PASS      b1
    #define REGISTER_LI_SB_MATERIAL_DATA t0
    #define REGISTER_LI_SB_INSTANCE_DATA t1

#endif // __CONSTS_SHARED__
