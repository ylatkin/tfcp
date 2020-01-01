# Building and Testing

TFCP consists of the public headers and the static library.  
You need to build the library before using the TFCP stuff.  
Build would also create tests and use examples.  

Your build script may look like following, e.g.:

    $ cd ~/code/tfcp
    $ mkdir -p build/linux
    $ cd build/linux
    $ cmake ../.. -DGTEST_ROOT=...

Here, the variable GTEST_ROOT points to the install folder  
of the Google test, which you need to build in advance, e.g.:

    $ cd ~/code/googletest
    $ mkdir -p build/linux
    $ cd build/linux
    $ cmake ../.. -DCMAKE_INSTALL_PREFIX=`pwd`/install
    $ make install

In this example:

    GTEST_ROOT = ~/code/googletest/build/linux/install

Get the clone of the Google test project from here:  
https://github.com/google/googletest
