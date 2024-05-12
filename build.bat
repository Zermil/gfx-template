@echo off

REM Change this to your visual studio's 'vcvars64.bat' script path
set MSVC_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"
set CFLAGS=/EHsc /W4 /WX /FC /wd4996 /wd4201 /wd4533 /wd4115 /wd4505 /nologo /fsanitize=address /Zi 
set CFLAGS_RELEASE=/EHsc /W4 /WX /FC /wd4996 /wd4533 /wd4201 /wd4115 /wd4505 /nologo /O2 /DNDEBUG /DUSE_OPTICK=0
set LIBS=user32.lib shell32.lib
set D3D11_LIBS=dxgi.lib dxguid.lib d3d11.lib d3dcompiler.lib
set OPENGL_LIBS=opengl32.lib

call %MSVC_PATH%\vcvars64.bat

pushd %~dp0
if not exist .\build mkdir build

if "%1" == "release" (
    cl %CFLAGS_RELEASE% "code\win32_d3d11_font_test.cpp" /Fo:build\ /Fe:build\win32_d3d11_app.exe /link %LIBS% %D3D11_LIBS%
    rem cl %CFLAGS_RELEASE% "code\win32_opengl_example.c" /Fo:build\ /Fe:build\win32_opengl_app.exe /link %LIBS% %OPENGL_LIBS%
    
    if exist ".\build\OptickCore.dll" del ".\build\OptickCore.dll"
    
    del ".\build\*.obj"
    del ".\build\*.pdb"
    del ".\build\*.exp"
    del ".\build\*.lib"
) else (
    cl %CFLAGS% "code\win32_d3d11_font_test.cpp" /Fo:build\ /Fe:build\win32_d3d11_app.exe /link %LIBS% %D3D11_LIBS% ".\external\OptickCore.lib"
    rem cl %CFLAGS% "code\win32_opengl_example.c" /Fo:build\ /Fe:build\win32_opengl_app.exe /link %LIBS% %OPENGL_LIBS% ".\external\OptickCore.lib"
    
    move ".\*.pdb" ".\build\"
    copy ".\external\OptickCore.dll" ".\build\"
)

popd
