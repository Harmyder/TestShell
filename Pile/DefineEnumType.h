#pragma once
#include "Print\DebugPrint.h"

#define DEFINE_ENUM_TYPE(x)                                                 \
    struct Type {                                                           \
        Type() : value_(x) {}                                               \
        explicit Type(uint32 value) : value_(value) {}                      \
        Type& operator=(const Type &o) { value_ = o.value_; return *this; } \
        uint32 PrivateValue() { return value_;}                             \
    private:                                                                \
        uint32 value_;                                                      \
    }
