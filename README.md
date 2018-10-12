# RVIZ Plugin for the HTC Vive

This Plugins allows RVIZ to render to the HTC Vive. This README is incomplete and will be updated in the near future.

## 1.0 Requirements
Please install SteamVR using the [Steam client](https://store.steampowered.com/about/). Clone the [OpenVR repository](https://github.com/ValveSoftware/openvr).

## 2.0 Build Instructions
The Plugin is build using [catkin](http://wiki.ros.org/catkin). You have to create a catkin workspace first. Open up a terminal and:

* *mkdir -p catkin_ws/src*
* *cd catkin_ws/src*
* *catkin_init_workspace*

Clone this repository into *catkin_ws/src*. Open the *CMakeList.txt* file and change the OpenVR path in line 30. You can then build the plugin by navigating to *catkin_ws* and executing *catkin_make* in a terminal.

## 3.0 Running the Plugin
To use the plugin, hookup your Vive, startup your ros environment, etc. Open up a new terminal and navigate to your catkin workspace. Then:

* *source devel/setup.zsh* or *source devel/setup.bash* depending on your shell

Start RVIZ *rosrun rviz rviz*, then press *Add* in the bottom left corner of the window, and add the *ViveDisplay*. Now the view in RVIZ should be rendered to your HTC Vive.

## 4.0 Credits
This Plugin was developed at the Institute for Advanced Mining Technologies, RWTH Aachen. It is based on this [previous project](https://github.com/AndreGilerson/rviz_vive_plugin)
