#pragma once

#include "Pile/Attribute/NonCopyable.h"

namespace Pile
{
    class Command : public Pile::NonCopyable
    {
    public:
        void SetReceiver(void *receiver)
        {
            m_Receiver = receiver;
        }

        virtual void Execute() = 0;

    protected:
        void *m_Receiver;
    };
}
