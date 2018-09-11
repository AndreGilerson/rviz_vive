/*Copyright (c) 2018 André Gilerson (andre.gilerson@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include "vive_display.h"

#include <rviz/display_context.h>
#include <rviz/view_manager.h>

#include <OGRE/OgreRoot.h>

#include <RenderSystems/GL/OgreGLTextureManager.h>

const float g_defaultIPD = 0.064f;

namespace rviz_vive
{

ViveDisplay::ViveDisplay()
{
	GLBounds = {};
	GLBounds.uMin = 0;
	GLBounds.uMax = 1;
	GLBounds.vMin = 1;
	GLBounds.vMax = 0;
}

ViveDisplay::~ViveDisplay()
{

}

void ViveDisplay::onInitialize()
{
    vr::EVRInitError eError = vr::VRInitError_None;
	_pHMD = vr::VR_Init( &eError, vr::VRApplication_Scene );

	if ( eError != vr::VRInitError_None )
	{
		_pHMD = NULL;
		std::cout << "Unable to init VR runtime";
    }

  	vr::EVRInitError peError = vr::VRInitError_None;
    if ( !vr::VRCompositor() )
	{
		std::cout << "Compositor initialization failed. See log file for details";
	}

	_pDisplayContext = context_;
    _pSceneManager = scene_manager_;
	_pSceneNode = _pSceneManager->getRootSceneNode()->createChildSceneNode();

    if (_pSceneNode)
		_pCameraNode = _pSceneNode->createChildSceneNode("StereoCameraNode");
	else
		_pCameraNode = _pSceneManager->getRootSceneNode()->createChildSceneNode("StereoCameraNode");

    _pCameras[0] = _pSceneManager->createCamera("CameraLeft");
	_pCameras[1] = _pSceneManager->createCamera("CameraRight");

	Ogre::GLTextureManager* textureManager = static_cast<Ogre::GLTextureManager*>(Ogre::TextureManager::getSingletonPtr());
    _renderTextures[0]= Ogre::TextureManager::getSingleton().createManual(
		"RenderTexture1", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		1512, 1680, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);
	_renderTextures[1] = Ogre::TextureManager::getSingleton().createManual(
		"RenderTexture2", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,
		1512, 1680, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);
	_pRenderTextures[0] = _renderTextures[0]->getBuffer()->getRenderTarget();
    _pRenderTexutresId[0] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RenderTexture1").getPointer())->getGLID();
    _pRenderTextures[1] = _renderTextures[1]->getBuffer()->getRenderTarget();
    _pRenderTexutresId[1] = static_cast<Ogre::GLTexture*>(textureManager->getByName("RenderTexture2").getPointer())->getGLID();

	vr::HmdMatrix44_t prj[2] = { 
		_pHMD->GetProjectionMatrix(vr::Eye_Left, 0.01, 100),
		_pHMD->GetProjectionMatrix(vr::Eye_Right, 0.01, 100)};

    for (int i = 0; i < 2; ++i)
	{
		_pCameras[i]->detachFromParent();
		_pCameraNode->attachObject(_pCameras[i]);

		_pCameras[i]->setCustomProjectionMatrix(true, MatSteamVRtoOgre4(prj[i]));

        _ports[i] = _pRenderTextures[i]->addViewport(_pCameras[i]);
        _ports[i]->setClearEveryFrame(true);
		_ports[i]->setBackgroundColour(Ogre::ColourValue::Black);
        _ports[i]->setOverlaysEnabled(false);
	}
}

void ViveDisplay::update(float wall_dt, float ros_dr)
{
	handleInput();

	Ogre::Camera *cam = _pDisplayContext->getViewManager()->getCurrent()->getCamera();
	Ogre::Vector3 pos = cam->getDerivedPosition();
	Ogre::Quaternion ori = cam->getDerivedOrientation();
	
	_pSceneNode->setOrientation(ori);
	_pSceneNode->setPosition(pos);

	if (_steamVrPose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		Ogre::Vector3 vivePos = _trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].getTrans();
		Ogre::Quaternion viveOri = _trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].extractQuaternion();
		_pCameraNode->setPosition(vivePos);
		_pCameraNode->setOrientation(ori);
	}

    _pRenderTextures[0]->update(true);
    _pRenderTextures[1]->update(true);

	if (_pHMD) 
	{
		vr::Texture_t leftEyeTexture = {(void*)_pRenderTexutresId[0], vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture,   &GLBounds );
		vr::Texture_t rightEyeTexture = {(void*)_pRenderTexutresId[1], vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture, &GLBounds);
    }
}

void ViveDisplay::reset()
{

}

Ogre::Matrix4 ViveDisplay::MatSteamVRtoOgre4(vr::HmdMatrix34_t matrix)
{
	return Ogre::Matrix4 (
		matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
		matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
		matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
		0.0, 0.0, 0.0, 1.0f
		);
}

Ogre::Matrix4 ViveDisplay::MatSteamVRtoOgre4(vr::HmdMatrix44_t matrix)
{
	return Ogre::Matrix4 (
		matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
		matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
		matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
		matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]
		);
}

void ViveDisplay::handleInput()
{
	vr::VRCompositor()->WaitGetPoses(_steamVrPose, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

	for (uint32_t nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
	{
		if (_steamVrPose[nDevice].bPoseIsValid)
		{
			_trackedDevicePose[nDevice] = MatSteamVRtoOgre4(_steamVrPose[nDevice].mDeviceToAbsoluteTracking).inverse();
		}
	}
}

};

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(rviz_vive::ViveDisplay, rviz::Display)
