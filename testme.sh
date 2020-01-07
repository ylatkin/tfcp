if [ "$1" == "" ]; then
    echo "#======================================================================="
    echo "#"
    echo "# Example of build-and-test script"
    echo "#"
    echo "# Builds TFCP library and tests and examples, and runs tests"
    echo "#"
    echo "# Compilers:"
    echo "# - GNU g++"
    echo "# - clang++"
    echo "# - icc (Intel C++)"
    echo "#"
    echo "# Configs:"
    echo "# - Release"
    echo "# - Debug"
    echo "#"
    echo "# Run it from from your build folder, and give it path to TFCP sources"
    echo "# E.g.:"
    echo "#"
    echo "#   cd \$TFCP_SOURCE_DIR"
    echo "#   mkdir -p build/linux"
    echo "#   cd build/linux"
    echo "#   ../../testme.sh \$TFCP_SOURCE_DIR"
    echo "#"
    echo "#======================================================================="
	exit 0
fi

TFCP_SOURCE_DIR=$1

#=======================================================================

execute() {
    echo $*
	$*
}

testme() {
    echo "#======================================================================="
    echo "#"
    echo "# Compiler: $1"
    echo "# Config: $2"
    echo "#"
    echo "#======================================================================="

    execute rm CMakeCache.txt

    execute cmake $TFCP_SOURCE_DIR -DCMAKE_CXX_COMPILER=$1 -DCMAKE_BUILD_TYPE=$2 || exit $?

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
