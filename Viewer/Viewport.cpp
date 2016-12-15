#include "stdafx.h"
#include "Viewport.h"

#include "Camera.h"
#include "CameraController.h"

using namespace std;

namespace Viewer
{
    Viewport::Viewport(HWND hWnd) :
        hwnd_(hWnd),
        cameraCtrl_(make_unique<CameraController>()),
        camera_(make_unique<Camera>())
    {
    }

    Viewport::~Viewport() {}


}