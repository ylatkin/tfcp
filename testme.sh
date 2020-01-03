
#=======================================================================
# 2019 (c) Evgeny Latkin
# License: Apache 2.0 (http://www.apache.org/licenses/)
#=======================================================================
#
# Example of build-and-test script
#
# Builds TFCP library and tests and examples, and runs tests
#
# Compilers:
# - GNU g++
# - clang++
# - icc (Intel C++)
#
# Configs:
# - Release
# - Debug
#
# Run this script from TFCP project's root folder
#
#=======================================================================

execute() {
    echo $*
	$*
}

echo "#"
echo "# Create build folder"
echo "#"
mkdir -p build/linux
execute cd build/linux || exit $?

#=======================================================================

testme() {
    echo "#"
    echo "# Compiler: $1"
    echo "# Config: $2"
    echo "#"

    execute rm CMakeCache.txt

    execute cmake ../.. -DCMAKE_CXX_COMPILER=$1 -DCMAKE_BUILD_TYPE=$2 || exit $?

    execute make -j4 || exit $?

    execute ctest || exit $?
}

#=======================================================================

testme g++ Release
testme g++ Debug

# NB: Intel C++ provices its own clang++
#     Test specifically /usr/bin/clang++
testme /usr/bin/clang++ Release
testme /usr/bin/clang++ Debug

testme icc Release
testme icc Debug

#=======================================================================
