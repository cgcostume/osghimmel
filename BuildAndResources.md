I (dm) am developing osghimmel with CMake 2.8.7, OSG 3.0.1 and Qt 4.8.0. Usually i prefer to build x64 binaries and since all mentioned libs and tools above are opensource, i build them my self. To bypass building third party libs yourself, this wikipage provides build instructions and lists available binary resources for each of them.



# CMake instructions #

There are currently 3 options:
  * OPTION\_MAKE\_DEMOS: adds sub-directories of demo apps
  * OPTION\_MAKE\_SKYBOX: add skybox project (osghimmel's sandbox)
  * OPTION\_MAKE\_PACKAGE: adds PACKAGE project to solution, yielding an archive with all available executables, required dependencies and resources. This **requires OSG\_BINARY\_DIR and OSG\_SOVERSION** (that is the number appended to the osg binary names - e.g. 80 in osg80.dll for 3.0.1).

When successfully building the PACKAGE project, a zip archive should be available in the build directory.

# Requirements and Binaries #

QtPropertyBrowser
  * Sources ([located here](http://qt.gitorious.org/qt-solutions/qt-solutions/trees/master/qtpropertybrowser)) are included in the trunk. No additional building is required.

CMake
  * Version **2.8 or higher** is recommended
  * Latest downloads can be found here [cmake](http://www.cmake.org/cmake/resources/software.html).

OpenSceneGraph
  * osghimmel requires [OpenSceneGraph](http://www.openscenegraph.org/projects/osg) tagged **2.8.5 or higher** (i'm using > 3.0).
  * Up-to-date sets of working binaries can be found at [Alpha Pixel](http://openscenegraph.alphapixel.com/osg/downloads/free-openscenegraph-binary-downloads).

Qt
  * The skybox (osghimmel's sandbox) requires Qt4 tagged **4.7 or higher**.
  * [qt-msvc-installer](http://code.google.com/p/qt-msvc-installer/downloads/list) provides several Microsoft Visual Studio builds.

# Build Instructions #

  1. **CMake - Configure and Verify**: After configuring CMake the first time, you should verify the OSG\_SOVERSION as well as the OSG\_BINARY\_DIR (since they are predefined or guessed).
  1. **Check Resources**: If Demos or Skybox option is enabled in CMake, a resources path should be specified ([downloads](http://code.google.com/p/osghimmel/downloads)). The content of this directory gets copied into the bin dir as post\_build step.
  1. **Build ALL**: After building ALL, binaries where placed into a bin directory, placed into the cmake\_source\_directory.
  1. **Build INSTALL** before running apps: Building the INSTALL target will copy all dependencies (Qt, OSG, etc) and resources (if any) into the bin directory.
  1. **Setup Working Directory**: set the Working Directory for executable targets in your e.g. MSVC projects to $(OutDir). This should be required only once for each project and configuration, since it is saved in the projects user file.
  1. **Run Apps**