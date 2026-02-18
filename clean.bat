@echo off
echo [Cleaning Otter DB State...]

:: Delete data chunks
if exist "./OFS/Buffer/bin/" del /Q ".\OFS\Buffer\bin\*.bin"
if exist "./Index/bin/" del /Q ".\Index\bin\*.bin"
if exist "./OFS/Buffer/config/" del /Q ".\OFS\Buffer\config\*.conf"
if exist "./OFS/WAL/bin" del /Q ".\OFS\WAL\bin\*.bin"
if exist "./StorageManager/InsertionQueue/bin/" del /Q ".\StorageManager\InsertionQueue\bin\*.bin"

echo [Clean Complete]