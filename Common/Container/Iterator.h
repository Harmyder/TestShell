#pragma once
#include <iterator>

namespace Common
{
    template <class Container, class T, class Derived>
    class Iterator : public std::iterator<std::forward_iterator_tag, T>
    {
    protected:
        using Me = Iterator;

        Container& container_;
        uint64 index_;

    public:
        explicit Iterator(Container& c) : container_(c), index_(0) {}
        Iterator(Container& c, uint32 i) : container_(c), index_(i) {}
        Iterator(const Derived& iter) : container_(iter.container_), index_(iter.index_) {}

        bool operator==(const Me& other) { AssertCompatibility(other); return &container_ == &other.container_ && index_ == other.index_; }
        bool operator!=(const Me& other) { AssertCompatibility(other); return !(*this == other); }
        bool operator<(const Me& other) { AssertCompatibility(other); return index_ < other.index_; }
        bool operator>(const Me& other) { AssertCompatibility(other); return index_ > other.index_; }
        Me& operator+=(difference_type off) { index_ += off; return (*this); }
        Derived operator+(difference_type off) const {
            Derived clone(*(Derived*)this); clone += off; return clone;
        }
        Me& operator-=(difference_type off) { index_ -= off; return (*this); }
        Derived operator-(difference_type off) const { Derived clone(*this); clone -= off; return clone; }

        virtual value_type& operator* () = 0;

        Me& operator++() { ++index_; return *this; }
        Derived operator++(int) { Derived clone(*this); ++clone.index_; return clone; }

    private:
        void AssertCompatibility(const Me& other) { if (&container_ != &other.container_) throw logical_error("Iterators are not compatible."); }
    };
}
