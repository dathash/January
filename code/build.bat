@echo off

if not exist build (mkdir build)
pushd build
cl -EHsc -nologo -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4189 -wd 4267 -wd4505 -wd4996 -FC -Z7 ..\january.cpp w:\january\lib\SDL2.lib w:\january\lib\SDL2_mixer.lib /I w:\january\include
popd
