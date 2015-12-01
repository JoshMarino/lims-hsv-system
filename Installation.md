# Introduction #

This section covers the steps required to get the code running in Visual Studio 2K8.

# Quick Note #

The instructions will mirror the setup on the current desktop that the code has been known to run on.  This does not mean that it will not work with a slightly modified configuration (i.e. Windows Vista vs Windows XP or Visual Studio 2K5 instead of VS2K8).

# Requirements #
  * Visual Studio 2008 Express Edition
  * Intel's OpenCV library (v1.0)
  * Silicon Software SDK library (v3.2)
  * Windows XP SP2

## Required for Online Camera Tracking ##
  * Silicon Softwre MicroEnable III (meIII) Frame Grabber
  * Photonfocus MV-D1024-TrackCam ("TrackCam") Camera
  * Photonfocus PFRemote (v2.5)
  * Desktop computer with an available PCI slot

# Installation #

## Hardware ##

Install all hardware as directed by the [Photonfocus](http://www.photonfocus.com/upload/manuals/user_manual_MV-D1024-Trackcam_REV_1_1.pdf) and [Silicon Software](http://www.silicon-software.com/download.html#_mE3) manuals.

## Software ##

  1. Download and install [Visual Studio 2K8](http://www.microsoft.com/express/vc/Default.aspx#webInstall)
  1. Download and install [OpenCV](http://sourceforge.net/project/showfiles.php?group_id=22870&package_id=16937)
  1. Download and install [meIII Driver and Software](http://www.silicon-software.com/download.html#_mE3)
  1. Download and install [PFRemote](http://www.photonfocus.com/html/eng/support/software.php)
  1. Checkout the [source code](http://code.google.com/p/lims-hsv-system/source/checkout)

## Verifying Installation ##
  1. Open GUIAndTiming.sln (this will load the project into VS2K8) in the GUIAndTiming folder
  1. If there is **no** camera attached to the system, then open fcdynamic.h in the "Header Files" folder found in the Solution Explorer side window and make sure there is a line that reads:
```
#define ONLINE 0
```
  1. Open main.cpp in the folder "Source Files" and make sure the following lines read:
```
#define TIMING 0
#define RECORDING 0
#define BOUNDING_BOX 128
```
  1. Press _F5_ (or Debug->Start Debugging) to run the program.  You should see a visual animation of a white cross appearing and disappearing inside a small 128x128 window.
  1. Press the '_h_' key to see instructions on how to use the GUI.  The instructions will appear in the command line window.

Now that you have a fully working installation feel free to modify or exclude main.cpp, time\_run.cpp, and display\_run.cpp from the Visual Studio project and build your own code.  Modifying other files may lead to unexpected behavior.

# Troubleshooting #
  * **The compiler or linker complains about missing header files or libraries**.  These errors are probably due to custom installation directories, to solve:
    * First, make sure all requirements have been installed properly
    * In VS2K8 edit the following fields in the Property Pages (_ALT+F7_) to reflect the actual directories on your system:
| Configuration Property | Sub Property | Field | Value |
|:-----------------------|:-------------|:------|:------|
| C/C++                  | General      | Additional Include Directories | "C:\Program Files\SiliconSoftware3.2\include";"C:\Program Files\OpenCV\cv\include";"C:\Program Files\OpenCV\cvaux\include";"C:\Program Files\OpenCV\otherlibs\highgui";"C:\Program Files\OpenCV\cvcore\include";"C:\Program Files\OpenCV\cxcore\include" |
| Linker                 | General      | Additional Library Directories | "C:\Program Files\OpenCV\lib";"C:\Program Files\SiliconSoftware3.2\lib\visualc" |
| Linker                 | Input        | Additional Dependencies | cv.lib cvcam.lib highgui.lib cxcore.lib cvaux.lib fglib3.1.lib display\_lib.lib fastconfig.lib clsersisome3.lib clserme3.lib |
| Linker                 | Input        | Ignore Specific Library | libmmdd.lib |

  * **The camera is installed correctly, but sometimes it hangs and no images are captured.  In fact, the timeout error code (-2120) is always being returned.**  This error is common when an incorrect parameter has been sent to the Photonfocus camera (e.g. RoiWidth or RoiPosX is a non-multiple of 4 or RoiWidth is < 8; see FastConfiguration documentation in the Silicon Software SDK for more information), to solve:
    1. Close all running applications that have access to the camera or frame grabber.
    1. Launch PFRemote from the Desktop and reset the camera to its factory defaults.
    1. Optionally, see if the camera is working normally in the microDisplay application, which is part of the Silicon Software SDK + Driver installation.
    1. Rerun your program.  It should now work.