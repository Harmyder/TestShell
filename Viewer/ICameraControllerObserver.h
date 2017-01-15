#pragma once

class ICameraControllerObserver {
public:
    virtual void HandleStartTracking() = 0;
    virtual void HandleStopTracking() = 0;
};
