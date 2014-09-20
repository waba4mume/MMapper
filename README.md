INSTALLING MMapper Version 2
============================

1.  You can get latest version of MMapper on http://github.com/mume/mmapper web page.
  
2.  Uncompress the archive you downloaded into any directory,
    e.g. C:\mmapper\ or ~/mmapper

3.  Dependencies
    You need to have a C++ compiler, CMake, and Qt and zlib development packages installed.

    On Fedora, you can get these by running:
    'yum install gcc-c++ cmake qt4-devel zlib-devel'

4.  Building

    Change the directory to the unpackage source tree, and run:

        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
        sudo make install

    Alternatively, you can use one of the wrappers:

        winbuild.bat
        ./build.sh

5.  That's all. MMapper is now installed.
    
6.  Set up your client according to the wiki instructions at:
    http://mume.org/wiki/index.php/Guide_to_install_mmapper2_on_Windows

We hope you will enjoy using MMapper. Good luck!
