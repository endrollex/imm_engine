cl /MD /O2 /W2 /c /DLUA_BUILD_AS_DLL *.c
ren lua.obj lua.o
ren luac.obj luac.o
link /DLL /IMPLIB:lua.lib /OUT:lua.dll *.obj
link /OUT:lua.exe lua.o lua.lib
lib /OUT:lua_static.lib *.obj
link /OUT:luac.exe luac.o lua.lib lua_static.lib