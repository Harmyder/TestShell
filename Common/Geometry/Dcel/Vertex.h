#pragma once

#include <Geometry/Dcel/Traits.h>

namespace Common
{
    namespace Dcel
    {
        template <class Id>
        class Vertex
        {
        public:
            void setLeaving(Id l) { _leaving = l; }
            Id getLeaving() const { return _leaving; }

        private:
            Id _leaving = Traits<Id>::kNoIndex;
        };
    }
}
