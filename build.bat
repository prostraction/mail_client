@echo off
setlocal

set VCPKG_DIR=%CD%\vcpkg
set VCPKG_TOOLCHAIN=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake
set CMAKE_BUILD_DIR=%CD%\build
set PROJECT_DIR=%CD%

:: Check and clone vcpkg if needed
if not exist "%VCPKG_DIR%" (
    echo Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git --branch=master
)

:: Check and bootstrap vcpkg if needed
if not exist "%VCPKG_DIR%\vcpkg.exe" (
    echo Bootstrapping vcpkg...
    "%VCPKG_DIR%\bootstrap-vcpkg.bat" -disableMetrics
	:: Install OpenSSL using vcpkg
	echo Installing OpenSSL using vcpkg...
	"%VCPKG_DIR%\vcpkg.exe" install openssl

	:: Configure CMake with vcpkg toolchain
	if not exist "%CMAKE_BUILD_DIR%" (
		mkdir "%CMAKE_BUILD_DIR%"
	)

	cmake -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" -G "Visual Studio 16 2019" "%PROJECT_DIR%"

	:: Build the project
	cmake --build "%CMAKE_BUILD_DIR%"

	:: Final message
	echo Build complete!
) else (
	:: Install OpenSSL using vcpkg
	echo Installing OpenSSL using vcpkg...
	"%VCPKG_DIR%\vcpkg.exe" install openssl

	:: Configure CMake with vcpkg toolchain
	if not exist "%CMAKE_BUILD_DIR%" (
		mkdir "%CMAKE_BUILD_DIR%"
	)

	cmake -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" -G "Visual Studio 16 2019" "%PROJECT_DIR%"

	:: Build the project
	cmake --build "%CMAKE_BUILD_DIR%"

	:: Final message
	echo Build complete!
)

endlocal
