@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=c:\Users\Tuana\Masa�st�\Projeler\C++ PROJELER\CustomEngine\CustomEngine\src;%path%;
mkdir C:\Users\tuana\Masa�st�\PROJELER\C++ PROJELER\CustomEngine\CustomEngine\src\build
pushd C:\Users\tuana\Masa�st�\PROJELER\C++ PROJELER\CustomEngine\CustomEngine\src\build
cl "C:\Users\tuana\Masa�st�\PROJELER\C++ PROJELER\CustomEngine\CustomEngine\src\main.cpp" user32.lib Gdi32.lib dinput8.lib dxguid.lib
popd
