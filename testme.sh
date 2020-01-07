
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

mkdir -p build/linux
execute cd build/linux || exit $?

#=======================================================================

testme() {
    echo "#======================================================================="
    echo "#"
    echo "# Compiler: $1"
    echo "# Config: $2"
    echo "#"
    echo "#======================================================================="

    execute rm CMakeCache.txt

    execute cmake ../.. -DCMAKE_CXX_COMPILER=$1 -DCMAKE_BUILD_TYPE=$2 || exit $?

    execute make -j4 || exit $?

    execute ctest || exit $?
}

#=======================================================================

testme g++ Release
testme g++ Debug

which_clang=`which clang++`
if [ "$which_clang" != "" ]; then
    testme clang++ Release
    testme clang++ Debug

    # Intel C++ may replace original clang++
    # Here we test original /usr/bin/clang++
    original_clang=/usr/bin/clang++
    if [ "$which_clang" != "$original_clang" ]; then
        testme $original_clang Release
        testme $original_clang Debug
    fi
fi

which_icc=`which icc`
if [ "$which_icc" != "" ]; then
    testme icc Release
    testme icc Debug
fi

#=======================================================================
