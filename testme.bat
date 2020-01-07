@if not "%1" == "" (
    set TFCP_SOURCE_DIR=%1
    goto TestCL
)

@echo #==================================================================
@echo #
@echo # Example of build-and-test script
@echo #
@echo # Builds TFCP library and tests and examples, and runs tests
@echo #
@echo # Compilers:
@echo # - Microsoft C++
@echo # - Intel C++
@echo #
@echo # Configs:
@echo # - Release
@echo # - Debug
@echo #
@echo # Run from your build folder and give it path to TFCP sources
@echo # E.g.:
@echo #   
@echo #   cd %%TFCP_SOURCE_DIR%%
@echo #   mkdir build\windows
@echo #   cd build\windows
@echo #   ..\..\testme.bat %%TFCP_SOURCE_DIR%%
@echo #
@echo #==================================================================

@goto Exit

:TestCL

@echo #=======================================================================
@echo #
@echo # Compiler: cl
@echo # Config: Release
@echo #
@echo #=======================================================================

del CMakeCache.txt
cmake %TFCP_SOURCE_DIR%
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
@rem # Reuse Visual Studio solution created by previous call to cmake
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
cmake %TFCP_SOURCE_DIR% -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
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
cmake %TFCP_SOURCE_DIR% -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
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
cmake %TFCP_SOURCE_DIR% -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=icl -DCMAKE_BUILD_TYPE=Release
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
cmake %TFCP_SOURCE_DIR% -G"NMake Makefiles" -DCMAKE_CXX_COMPILER=icl -DCMAKE_BUILD_TYPE=Debug
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
