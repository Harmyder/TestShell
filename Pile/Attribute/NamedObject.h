#pragma once

namespace Pile
{

    class NamedObject
    {
    protected:
        NamedObject(const char* name) : name_(name) {}

    public:
        const char* GetName() const { return name_; };

    private:
        const char* name_;
    };

}
