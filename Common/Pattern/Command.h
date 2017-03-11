#pragma once

#include "Common/Attribute/NonCopyable.h"

namespace Common
{
    class Command : public Common::NonCopyable
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
