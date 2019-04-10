@SETLOCAL

@set ZIP="C:\Program Files\7-Zip\7z.exe"
@set OUTPUT=.\build\ModEngine.zip

@call .\build.bat clear || @goto fail
@call .\build.bat engine_debug engine_release include || @goto fail

@for %%G in (".\build\debug" ".\build\release" ".\build\include") do @(
  call %ZIP% a "%OUTPUT%" %%G || @goto fail
)

@echo Packaging complete
@exit /b 0

:fail
  @echo Packaging failed
  @exit /b 1