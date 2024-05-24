@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=c:\Users\Tuana\Masaüstü\Projeler\C++ PROJELER\CustomEngine\CustomEngine\src;%path%;
mkdir C:\Users\tuana\Masaüstü\PROJELER\C++ PROJELER\CustomEngine\CustomEngine\src\build
pushd C:\Users\tuana\Masaüstü\PROJELER\C++ PROJELER\CustomEngine\CustomEngine\src\build
cl "C:\Users\tuana\Masaüstü\PROJELER\C++ PROJELER\CustomEngine\CustomEngine\src\main.cpp" user32.lib Gdi32.lib dinput8.lib dxguid.lib
popd
