for %%i in (*.png) do BC6HBC7EncoderCS.exe /bc7 "%%i"
for %%i in (*.jpg) do BC6HBC7EncoderCS.exe /bc7 "%%i"
for %%i in (*.tif) do BC6HBC7EncoderCS.exe /bc7 "%%i"
for %%i in (*.tga) do BC6HBC7EncoderCS.exe /bc7 "%%i"
@echo off
echo Delete filename _BC7 suffix:
for /f "delims=" %%i in ('dir /b *_BC7.dds') do (
set s=%%i
call echo %%s:~0,-8%%.dds
call ren %%i %%s:~0,-8%%.dds
)
@echo on
pause
