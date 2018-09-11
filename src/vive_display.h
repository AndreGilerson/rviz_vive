/*Copyright (c) 2018 André Gilerson (andre.gilerson@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#pragma once

#ifndef Q_MOC_RUN
#include "rviz/display.h"
#endif

#include <OGRE/OgreTexture.h>
#include <OGRE/RenderSystems/GL/OgreGLTexture.h>

#include <openvr.h>

namespace rviz_vive
{

class ViveDisplay: public rviz::Display
{
Q_OBJECT
public:
    ViveDisplay();
    virtual ~ViveDisplay();

    virtual void onInitialize();
    virtual void update(float wall_dt, float ros_dt);
    virtual void reset();
private:
    Ogre::Matrix4 MatSteamVRtoOgre4(vr::HmdMatrix34_t matrix);
	Ogre::Matrix4 MatSteamVRtoOgre4(vr::HmdMatrix44_t matrix);

    void handleInput();

   	vr::IVRSystem* _pHMD;

	rviz::DisplayContext* _pDisplayContext;
   	Ogre::SceneManager* _pSceneManager;

    Ogre::Camera* _pCameras[2];
    Ogre::SceneNode* _pSceneNode;
    Ogre::SceneNode* _pCameraNode;
	Ogre::Viewport* _ports[2];
	Ogre::TexturePtr _renderTextures[2];
	Ogre::RenderTexture* _pRenderTextures[2];
    GLuint _pRenderTexutresId[2];

    vr::VRTextureBounds_t GLBounds;

    vr::TrackedDevicePose_t _steamVrPose[vr::k_unMaxTrackedDeviceCount];
    Ogre::Matrix4 _trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
};

};
