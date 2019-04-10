@echo off
SETLOCAL EnableDelayedExpansion



:: Flags to pass to the compiler
set STD_FLAGS=-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -Wno-pragma-pack -Xclang -fexceptions -Xclang -fcxx-exceptions -std:c++17
set DBG_FLAGS=-DDEBUG -Z7
set REL_FLAGS=-DRELEASE -Ofast -Z7



:: Handle action dispatch
if "%1" == "" (
  echo Build script requires the names of build tasks you would like to run
  echo For example to build the test game and dependencies, run ".\build.bat engine_debug include test_game test_mod"
  exit /b 1
) else (
  if "%1" == "clear" (
    set TAIL=%*
    call :clear %%TAIL:*%1=%%
  ) else (
    for %%i in (%*) do (
      call :%%i || (
        echo.
        echo [91mTask %%i failed[0m
        echo.
        exit /b 1
      )
      echo.
      echo [92mTask %%i complete[0m
      echo.
    )
  )

  exit /b 0
)


:include
  :: Remove old content
  if EXIST .\build\include rmdir /s /q .\build\include

  :: Prepare directories
  mkdir .\build\include
  mkdir .\build\include\extern

  :: Copy engine include files and license
  robocopy .\source\core\include .\build\include /e /NFL /NJH /NJS /nc /ns /np & if %ERRORLEVEL% GEQ 1 exit /b 1
  copy LICENSE .\build\include\LICENSE || exit /b 1

  :: Copy dependency include files and licenses
  for /f "tokens=*" %%G in ('dir /b .\source\extern') do (
    mkdir .\build\include\extern\%%G || exit /b 1
    if EXIST .\source\extern\%%G\include (
      mkdir .\build\include\extern\%%G\include || exit /b 1
      robocopy .\source\extern\%%G\include .\build\include\extern\%%G\include /e /NFL /NJH /NJS /nc /ns /np & if %ERRORLEVEL% GEQ 1 exit /b 1
    ) else (
      copy .\source\extern\%%G\*.h .\build\include\extern\%%G\ || exit /b 1
    )
    for %%i in (LICENSE UNLICENSE) do (
      if EXIST .\source\extern\%%G\%%i (
        copy .\source\extern\%%G\%%i .\build\include\extern\%%G\%%i || exit /b 1
      )
    )
  )

  exit /b 0


:engine_debug
  :: Setup execution env
  call :get_tools || exit /b 1

  :: Remove old content
  if EXIST .\build\debug rmdir /s /q .\build\debug

  :: Prepare directory
  mkdir .\build\debug

  :: Build engine dll
  clang-cl -DM_ENGINE ^
    %STD_FLAGS% %DBG_FLAGS% -LD ^
    -I.\source\ -I.\source\core\include\ ^
    .\source\core\implementation\ModEngine.cc ^
    .\source\extern\sdl2\sdl2.lib .\source\extern\freeimage\FreeImage.lib .\source\extern\imgui\imgui.lib ^
    -o.\build\debug\ModEngine ^
  || exit /b 1

  :: Merge dependency libs into engine lib
  lib ^
    .\build\debug\ModEngine.lib ^
    .\source\extern\sdl2\sdl2.lib .\source\extern\freeimage\FreeImage.lib .\source\extern\imgui\imgui.lib ^
    /OUT:.\build\debug\ModEngine.lib ^
  || exit /b 1

  :: Copy dependency dlls into engine directory
  copy .\source\extern\sdl2\sdl2_dbg.dll .\build\debug\sdl2.dll
  copy .\source\extern\freeimage\FreeImage.dll .\build\debug\FreeImage.dll
  copy .\source\extern\imgui\imgui_dbg.dll .\build\debug\imgui.dll

  :: Copy dependency pdbs into engine directory
  copy .\source\extern\imgui\imgui.pdb .\build\debug\imgui.pdb

  :: Clean up intermediate files
  for %%i in (
    .\build\debug\*.ilk ^
    .\build\debug\*.exp
  ) do (
    if EXIST %%i del %%i
  )
  
  exit /b 0


:engine_release
  :: Setup execution env
  call :get_tools || exit /b 1

  :: Remove old content
  if EXIST .\build\release rmdir /s /q .\build\release

  :: Prepare directory
  mkdir .\build\release

  :: Build engine dll
  clang-cl -DM_ENGINE ^
    %STD_FLAGS% %REL_FLAGS% -LD ^
    -I.\source\ -I.\source\core\include\ ^
    .\source\core\implementation\ModEngine.cc ^
    .\source\extern\sdl2\sdl2.lib .\source\extern\freeimage\FreeImage.lib .\source\extern\imgui\imgui.lib ^
    -o.\build\release\ModEngine ^
  || exit /b 1

  :: Merge dependency libs into engine lib
  lib ^
    .\build\release\ModEngine.lib ^
    .\source\extern\sdl2\sdl2.lib .\source\extern\freeimage\FreeImage.lib .\source\extern\imgui\imgui.lib ^
    /OUT:.\build\release\ModEngine.lib ^
  || exit /b 1

  :: Copy dependency dlls into engine directory
  copy .\source\extern\sdl2\sdl2_rel.dll .\build\release\sdl2.dll
  copy .\source\extern\freeimage\FreeImage.dll .\build\release\FreeImage.dll
  copy .\source\extern\imgui\imgui_rel.dll .\build\release\imgui.dll

  :: Clean up intermediate files
  for %%i in (
    .\build\release\*.ilk ^
    .\build\release\*.exp
  ) do (
    if EXIST %%i del %%i
  )
  
  exit /b 0


:test_game_debug
  :: Make sure other tasks have been run first
  if NOT EXIST .\build\debug (
    echo Build task 'test_game_debug' requires tasks 'engine_debug' and 'include' to be run first
    exit /b 1
  )
  
  if NOT EXIST .\build\include (
    echo Build task 'test_game_debug' requires tasks 'engine_debug' and 'include' to be run first
    exit /b 1
  )

  :: Setup execution env
  call :get_tools || exit /b 1

  :: Remove old content
  if EXIST .\build\test\debug rmdir /s /q .\build\test\debug

  :: Prepare directories
  mkdir .\build\test\debug

  :: Build test game exe
  clang-cl -DM_GAME ^
    %STD_FLAGS% %DBG_FLAGS% ^
    -I.\build\include\ ^
    .\source\test\main.cc ^
    .\build\debug\ModEngine.lib ^
    -o.\build\test\debug\main ^
  || exit /b 1

  if EXIST .\build\test\debug\main.lib (
    :: Merge engine lib into test game lib if it exists
    lib ^
      .\build\test\debug\main.lib ^
      .\build\debug\ModEngine.lib ^
      /OUT:.\build\test\debug\main.lib ^
    || exit /b 1
  ) else (
    :: Copy engine lib and rename it for use by test_mod as the test game lib
    copy .\build\debug\ModEngine.lib .\build\test\debug\main.lib
  )

  :: Copy debug dlls into test game directory
  copy .\build\debug\*.dll .\build\test\debug

  :: Copy debug pdb files into test game directory
  copy .\build\debug\*.pdb .\build\test\debug

  :: Clean up intermediate files
  for %%i in (
    .\build\test\debug\*.ilk ^
    .\build\test\debug\*.exp
  ) do (
    if EXIST %%i del %%i
  )

  exit /b 0


:test_game_release
  :: Make sure other tasks have been run first
  if NOT EXIST .\build\release (
    echo Build task 'test_game_release' requires tasks 'engine_release' and 'include' to be run first
    exit /b 1
  )
  
  if NOT EXIST .\build\include (
    echo Build task 'test_game_release' requires tasks 'engine_release' and 'include' to be run first
    exit /b 1
  )

  :: Setup execution env
  call :get_tools || exit /b 1

  :: Remove old content
  if EXIST .\build\test\release rmdir /s /q .\build\test\release

  :: Prepare directories
  mkdir .\build\test\release

  :: Build test game exe
  clang-cl -DM_GAME ^
    %STD_FLAGS% %REL_FLAGS% ^
    -I.\build\include\ ^
    .\source\test\main.cc ^
    .\build\release\ModEngine.lib ^
    -o.\build\test\release\main ^
  || exit /b 1

  if EXIST .\build\test\release\main.lib (
    :: Merge engine lib into test game lib if it exists
    lib ^
      .\build\test\release\main.lib ^
      .\build\release\ModEngine.lib ^
      /OUT:.\build\test\release\main.lib ^
    || exit /b 1
  ) else (
    :: Copy engine lib and rename it for use by test_mod as the test game lib
    copy .\build\release\ModEngine.lib .\build\test\release\main.lib
  )

  :: Copy release dlls into test game directory
  copy .\build\release\*.dll .\build\test\release

  :: Clean up intermediate files
  for %%i in (
    .\build\test\release\*.ilk ^
    .\build\test\release\*.exp
  ) do (
    if EXIST %%i del %%i
  )

  exit /b 0


:test_mod_debug
  :: Make sure other tasks have been run first
  if NOT EXIST .\build\test\debug (
    echo Build task 'test_mod_debug' requires tasks 'test_game_debug' and 'include' to be run first
    exit /b 1
  )

  if NOT EXIST .\build\include (
    echo Build task 'test_mod_debug' requires tasks 'test_game_debug' and 'include' to be run first
    exit /b 1
  )

  :: Setup execution env
  call :get_clang || exit /b 1

  :: Remove old content
  if EXIST .\build\test\debug\mods rmdir /s /q .\build\test\debug\mods

  :: Prepare directories
  mkdir .\build\test\debug\mods

  :: Build test modification dll
  clang-cl -DM_MODULE ^
    %STD_FLAGS% %DBG_FLAGS% -LD ^
    -I.\build\include ^
    source\test\mod\mod.cc ^
    .\build\test\debug\main.lib ^
    -o.\build\test\debug\mods\mod ^
  || exit /b 1

  :: Clean up intermediate files
  for %%i in (
    .\build\test\debug\mods\*.ilk ^
    .\build\test\debug\mods\*.exp ^
    .\build\test\debug\mods\mod.lib
  ) do (
    if EXIST %%i del %%i
  )

  exit /b 0


:test_mod_release
  :: Make sure other tasks have been run first
  if NOT EXIST .\build\test\release (
    echo Build task 'test_mod_release' requires tasks 'test_game_release' and 'include' to be run first
    exit /b 1
  )

  if NOT EXIST .\build\include (
    echo Build task 'test_mod_release' requires tasks 'test_game_release' and 'include' to be run first
    exit /b 1
  )

  :: Setup execution env
  call :get_clang || exit /b 1

  :: Remove old content
  if EXIST .\build\test\release\mods rmdir /s /q .\build\test\release\mods

  :: Prepare directories
  mkdir .\build\test\release\mods

  :: Build test modification dll
  clang-cl -DM_MODULE ^
    %STD_FLAGS% %REL_FLAGS% -LD ^
    -I.\build\include ^
    source\test\mod\mod.cc ^
    .\build\test\release\main.lib ^
    -o.\build\test\release\mods\mod ^
  || exit /b 1

  :: Clean up intermediate files
  for %%i in (
    .\build\test\release\mods\*.ilk ^
    .\build\test\release\mods\*.exp ^
    .\build\test\release\mods\mod.lib
  ) do (
    if EXIST %%i del %%i
  )

  exit /b 0



:run_debug
  start /b /wait .\build\test\debug\main.exe
  if NOT %ERRORLEVEL%==0 (
    echo.
    echo [91mDebug application failed: Error code %ERRORLEVEL%[0m
  )
  exit /b %ERRORLEVEL%


:run_release
  start /b /wait .\build\test\release\main.exe
  if NOT %ERRORLEVEL%==0 (
    echo.
    echo [91mRelease application failed: Error code %ERRORLEVEL%[0m
  )
  exit /b %ERRORLEVEL%



:clear
  :: Remove build directories
  if "%1" == "" (
    if EXIST .\build (
      echo Clearing build directory
      rmdir /s /q .\build
    ) else (
      echo Build directory already cleared
    )
  ) else (
    for %%i in (%*) do (
      if EXIST .\build\%%i (
        echo Clearing directory "%%i"
        rmdir /s /q .\build\%%i
      ) else (
        echo Directory "%%i" already cleared
      )
    )
  )

  exit /b 0
  


:get_tools
  :: Ensure both clang-cl and Windows' build tools are available
  call :get_clang || exit /b 1
  call :get_msvc || exit /b 1

  exit /b 0


:get_clang
  :: Ensure clang-cl is available
  if "%CLANG%" NEQ "1" (
    where /q clang-cl
    if %ERRORLEVEL% GEQ 1 (
      echo Build script requires a version of clang-cl to be installed on the system path. You can download an installer or source code from http://releases.llvm.org/download.html
      exit /b 1
    )
    set CLANG=1
  )

  exit /b 0


:get_msvc
  :: Prepare env variables for access to Windows' build tools
  set VC_VARS=vcvars64.bat
  set MS_DIRECTORIES="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build" "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\"
  if "%MSVC%" NEQ "1" (
    for %%i in (!MS_DIRECTORIES!) do (
      if EXIST "%%~i\%VC_VARS%" (
        call "%%~i\%VC_VARS%"
        set MSVC=1
        exit /b 0
      )
    )
    echo Need a copy of either Visual Studio 2017, or the standalone Build Tools for Visual Studio 2017
    echo Checked the following directories:
    for %%i in (!MS_DIRECTORIES!) do echo %%i
    echo Visual Studio Community and/or Build Tools can be downloaded for free from https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2017
    exit /b 1
  )
  
  exit /b 0