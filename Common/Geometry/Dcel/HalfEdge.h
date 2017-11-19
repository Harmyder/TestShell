#pragma once

#include <Geometry/Dcel/Traits.h>

namespace Common
{
    namespace Dcel
    {
        template <class Id>
        class HalfEdge
        {
        public:
            void setOrigin(Id o) { _origin = o; }
            Id getOrigin() const { return _origin; }
            void setNext(Id n) { _next = n; }
            Id getNext() const { return _next; }
            void setFace(Id f) { _face = f; }
            Id getFace() const { return _face; }

            bool operator==(const HalfEdge& other) const {
                return (_origin == other._origin && _next == other._next && _face == other._face);
            }

        private:
            Id _origin = Traits<Id>::kNoIndex;
            Id _next = Traits<Id>::kNoIndex;
            Id _face = Traits<Id>::kNoIndex;
        };
    }
}
