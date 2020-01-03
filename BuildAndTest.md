# Building and Testing

TFCP consists of the public headers and the static library.  
You need to build the library before using the TFCP stuff.  
Build would also create tests and use examples.  

Your build script may look like following, e.g.:

    $ cd ~/code/tfcp
    $ mkdir -p build/linux
    $ cd build/linux
    $ cmake ../..
    # make
