# How to build from source

## On Windows

1. Install Visual Studio Community 2015 or later with the optional feature "Visual C++".

2. Download Boost C++ libraries, and run the following or similar command to build them:
~~~
"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
.\bootstrap.bat
.\b2.exe define=BOOST_USE_WINAPI_VERSION=0x0600 variant=debug,release link=static runtime-link=static threading=multi toolset=msvc-14.0 -j2 stage
~~~

3. Download the source code of wxWidgets, and open build\msw\wx_vc14.sln, and change an option /MD to /MT in Properties->C/C++->Code Generation->Runtime Library of the Release configuration on each project.

4. Build wxWidgets in the Release configuration.

5. Download the source code of Zero Problem, open src\ZeroProblem.sln, select *x86* from the active solution platform combo box, and correct the values of the properties of include and library directories in Properties -> VC++ Directories.

6. Build Zero Problem in the Release configuration.

## On Linux

Zero Problem has not been tested enough on Linux, but you can build the software with the following or similar command if you are lucky:
~~~
sudo apt install build-essential git dpkg-deb-dev cmake libwxgtk3.0-dev libboost-all-dev fonts-roboto
cd ~
git clone https://github.com/colonq/zero-problem
cd zero-problem/src
mkdir build
cd build
cmake ..
make
~~~
and install the software like below:
~~~
cpack
sudo apt install ./zero-problem*.deb
cd ..
rm -rf build
~~~
and uninstall it like below:
~~~
sudo apt remove zero-problem
~~~
