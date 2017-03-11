#pragma once

#include "Common\Pattern\Singleton.h"

namespace Harmyder
{
    class RididBody;

    class HarmyderModule {
        DECLARE_CLASS_SINGLETON(HarmyderModule)

    public:
        void Init();
        void Close();
    };
}