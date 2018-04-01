#pragma once
#include <iterator>

namespace Common
{
    template <class Container, class T, class Derived>
    class ForwardIterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        explicit ForwardIterator(Container& c) : container_(c), index_(0) {}
        ForwardIterator(Container& c, uint32 i) : container_(c), index_(i) {}
        ForwardIterator(const Derived& iter) : container_(iter.container_), index_(iter.index_) {}

        using Me = ForwardIterator;

        bool operator==(const Me& other) { AssertCompatibility(other); return &container_ == &other.container_ && index_ == other.index_; }
        bool operator!=(const Me& other) { AssertCompatibility(other); return !(*this == other); }

        virtual value_type& operator* () = 0;
        virtual value_type* operator->() = 0;

        Me& operator++() { ++index_; return *this; }
        Derived operator++(int) { Derived clone(*this); ++clone.index_; return clone; }

    protected:
        void AssertCompatibility(const Me& other) { if (&container_ != &other.container_) throw logical_error("Iterators are not compatible."); }

        Container& container_;
        uint64 index_;
    };

    template <class Container, class T, class Derived>
    class BidirectionalIterator : public ForwardIterator<Container, T, Derived>
    {
    public:
        using ForwardIterator::ForwardIterator;

        using Me = BidirectionalIterator;

        Me& operator--() { --index_; return *this; }
        Derived operator--(int) { Derived clone(*this); --clone.index_; return clone; }
    };

    template <class Container, class T, class Derived>
    class RandomAccessIterator : public BidirectionalIterator<Container, T, Derived>
    {
    public:
        using BidirectionalIterator::BidirectionalIterator;

        using Me = RandomAccessIterator;

        bool operator<(const Me& other) { AssertCompatibility(other); return index_ < other.index_; }
        bool operator>(const Me& other) { AssertCompatibility(other); return index_ > other.index_; }

        Me& operator+=(difference_type off) { index_ += off; return (*this); }
        Me& operator-=(difference_type off) { index_ -= off; return (*this); }
        Derived operator+(difference_type off) const { Derived clone(*(Derived*)this); clone += off; return clone; }
        Derived operator-(difference_type off) const { Derived clone(*this); clone -= off; return clone; }
    };
}
