::edit the path
::
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%;D:\Ashlotte\include_imm;
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%D:\Ashlotte\include_d3d11book;
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%D:\Ashlotte\include_dependent;
@if not "%IMM_SET%"=="1" set IMM_LIB=%IMM_LIB%;D:\Ashlotte\lib_binary;
@if not "%IMM_SET%"=="1" set IMM_INCLUDE=%IMM_INCLUDE%C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include;
@if not "%IMM_SET%"=="1" set INCLUDE=%INCLUDE%%IMM_INCLUDE%
@if not "%IMM_SET%"=="1" set LIB=%LIB%%IMM_LIB%
@set IMM_SET=1
::Debug
cl /EHsc /MDd /Z7 /W4 /fp:fast /bigobj main.cc /link /ignore:4099
::Relase
::cl /EHsc /MD /Ox /W4 /fp:fast /GL main.cc