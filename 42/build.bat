@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set CFLAGS=-FC -Od -MTd -nologo -fp:fast -fp:except- ^
  -Gm- -GR- -EHsc -EHa- -Z7 -Oi ^
  -WX -W4 -wd4100 -wd4189 -wd4201 -wd4244 -wd4459 -wd4505 -wd4214 -wd4204 ^
  -DUNICODE=1 -DDEVELOPMENT=1 ^
    -I..\..\external\stb\include 

set LFLAGS=-incremental:no -opt:ref ^
  user32.lib gdi32.lib opengl32.lib dsound.lib dxguid.lib

IF NOT EXIST build\ mkdir build\
pushd build\

del *.pdb > NUL 2> NUL

::rc /fo runtime.res ..\source\win32\resource.rc
cl %CFLAGS% -TC ..\source\ludumdare.c -LD /link -incremental:no -opt:ref
cl %CFLAGS% -TC ..\source\main.c /link %LFLAGS% runtime.res /out:runtime.exe 

popd
