#pragma once

#define DECLARE_CLASS_SINGLETON(S)         \
    static S    instance;                  \
                                           \
    public:                                \
        static S& GetInstance()            \
        {                                  \
            return instance;               \
        }                                  \
        S(S const&) = delete;              \
        void operator=(S const&) = delete; \
                                           \
    private:                               \
        S();                               \
        
#define DEFINE_SINGLETON(S)   S S::instance; \
                              S::S() {}