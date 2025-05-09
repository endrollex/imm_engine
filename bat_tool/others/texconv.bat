for %%i in (*.png) do texconv.exe -f BC7_UNORM "%%i" -y
for %%i in (*.jpg) do texconv.exe -f BC7_UNORM "%%i" -y
for %%i in (*.tif) do texconv.exe -f BC7_UNORM "%%i" -y
for %%i in (*.tga) do texconv.exe -f BC7_UNORM "%%i" -y
@echo off
echo Rename:
for /f "delims=" %%i in ('dir /b *.dds') do (
set s=%%i
call echo %%s:~0,-4%%.dds
call ren %%i %%s:~0,-4%%.dds
)
@echo on
pause
