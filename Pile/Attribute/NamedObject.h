#pragma once

namespace Pile
{

    class NamedByRefObject
    {
    protected:
        NamedByRefObject(const std::string& name) : name_(name) {}

    public:
        const std::string& GetName() const { return name_; };

    private:
        const std::string& name_;
    };

    class NamedByCopyObject
    {
    protected:
        NamedByCopyObject(const std::string& name) : name_(name) {}

    public:
        const std::string& GetName() const { return name_; };

    private:
        const std::string name_;
    };
}
