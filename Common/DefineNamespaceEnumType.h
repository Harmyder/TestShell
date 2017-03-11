#pragma once
#include "Print\DebugPrint.h"

#define DEFINE_NAMESPACE_ENUM_TYPE(underlyingType, defaultValue)   \
    class Type : public TypeSpecific<underlyingType, defaultValue> \
        { using TypeSpecific::TypeSpecific; }

#define CHECK_NAMESPACE_ENUM_TYPE(namespaceSrc)                                              \
static_assert(std::is_same<Type::UnderlyingType, namespaceSrc::Type::UnderlyingType>::value, \
    "Type::UnderlyingType is different from " #namespaceSrc "::Type::UnderlyingType");       \
static_assert(Type::kDefault == namespaceSrc::Type::kDefault,                                \
    "Type::kDefault does not equal to " #namespaceSrc "::Type::kDefault")

#define DEFINE_NAMESPACE_ENUM_MEMBER(namespaceSrc, name) \
    Type name() { return Type(*(uint32*)&namespaceSrc::name); }

#define DEFINE_NAMESPACE_ENUM_TOSRC(namespaceSrc) \
    namespaceSrc::Type ToSrc(Type t) { return *(namespaceSrc::Type*)&t; }

template <class UT, UT value>
class TypeSpecific {
public:
    TypeSpecific() : value_(value) {}
    explicit TypeSpecific(UT value) : value_(value) {}
    explicit operator UT() const { return value_; }

    TypeSpecific& operator=(const TypeSpecific &o) { value_ = o.value_; return *this; }

    using UnderlyingType = UT;
    static constexpr UT kDefault = value;
private:
    UT value_;
};
