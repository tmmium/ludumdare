@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set CommonCompilerFlags=-FC -Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHsc -EHa- -Z7 -Oi ^
  -WX -W4 -wd4100 -wd4189 -wd4201 -wd4244 -wd4459 -wd4505 ^
  -DUNICODE=1 -DDEVELOPMENT=1 ^
  -I..\..\external\stb\include 
set CommonLinkerFlags=-incremental:no -opt:ref ^
  broccoli.lib

IF NOT EXIST build\ mkdir build\
pushd build\

del *.pdb > NUL 2> NUL

cl %CommonCompilerFlags% -TC ..\source\broccoli.c -LD /link -incremental:no -opt:ref 
cl %CommonCompilerFlags% ..\source\main.cc /link %CommonLinkerFlags% /out:ludumdarefourty.exe

popd
