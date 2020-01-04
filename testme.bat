@rem #==================================================================
@rem # 2019 (c) Evgeny Latkin
@rem # License: Apache 2.0 (http://www.apache.org/licenses/)
@rem #==================================================================
@rem #
@rem # Example of build-and-test script
@rem #
@rem # Builds TFCP library and tests and examples, and runs tests
@rem #
@rem # Compilers:
@rem # - Microsoft C++
@rem # - Intel C++
@rem #
@rem # Configs:
@rem # - Release
@rem # - Debug
@rem #
@rem # Run this script from TFCP project's root folder
@rem #
@rem #==================================================================

@rem Current directory
@set TFCP_ROOT=%cd%

mkdir build
mkdir build\windows
cd build\windows
@if not %errorlevel% == 0 goto Error

@echo #=======================================================================
@echo #
@echo # Compiler: cl
@echo # Config: Release
@echo #
@echo #=======================================================================

del CMakeCache.txt
cmake ..\..
@if not %errorlevel% == 0 goto Error

cmake --build . --config Release
@if not %errorlevel% == 0 goto Error

ctest -C Release
@if not %errorlevel% == 0 goto Error

@echo #=======================================================================
@echo #
@echo # Compiler: cl
@echo # Config: Debug
@echo #
@echo #=======================================================================

@rem #
@rem # NB: reuse Visual Studio solution created by previous call to cmake
@rem #

cmake --build . --config Debug
@if not %errorlevel% == 0 goto Error

ctest -C Debug
@if not %errorlevel% == 0 goto Error

@rem #========================================================================
@rem #
@rem # Intel Parallel Studio may additionally provide clang/clang++ compiler
@rem # 
@rem #========================================================================

:TestClang

clang++ --version >NUL
@if not %errorlevel% == 0 goto TestICL

@rem # TODO: fix testing with clang++
goto TestICL

@echo #=======================================================================
@echo #
@echo # Compiler: clang++
@echo # Config: Release
@echo #
@echo #=======================================================================

del CMakeCache.txt
cmake ..\.. -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
@if not %errorlevel% == 0 goto Error

nmake
@if not %errorlevel% == 0 goto Error

ctest
@if not %errorlevel% == 0 goto Error

@echo #=======================================================================
@echo #
@echo # Compiler: clang++
@echo # Config: Debug
@echo #
@echo #=======================================================================

del CMakeCache.txt
cmake ..\.. -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
@if not %errorlevel% == 0 goto Error

nmake
@if not %errorlevel% == 0 goto Error

ctest
@if not %errorlevel% == 0 goto Error

@rem #========================================================================
@rem #
@rem # Intel Parallel Studio is optional, so check Intel C++ is available
@rem # 
@rem #========================================================================

:TestICL

icl /help >NUL
@if not %errorlevel% == 0 goto Exit

@echo #=======================================================================
@echo #
@echo # Compiler: icl
@echo # Config: Release
@echo #
@echo #=======================================================================

del CMakeCache.txt
cmake ..\.. -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=icl -DCMAKE_BUILD_TYPE=Release
@if not %errorlevel% == 0 goto Error

nmake
@if not %errorlevel% == 0 goto Error

ctest
@if not %errorlevel% == 0 goto Error

@echo #=======================================================================
@echo #
@echo # Compiler: icl
@echo # Config: Debug
@echo #
@echo #=======================================================================

del CMakeCache.txt
cmake ..\.. -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=icl -DCMAKE_BUILD_TYPE=Debug
@if not %errorlevel% == 0 goto Error

nmake
@if not %errorlevel% == 0 goto Error

ctest
@if not %errorlevel% == 0 goto Error

@rem #==================================================================

@goto Exit

:Error
@echo #
@echo # ERROR: exiting abnormally
@echo #

:Exit
@rem Exit silently
@cd %TFCP_ROOT%
