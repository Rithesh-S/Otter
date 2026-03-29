@echo off
echo [Cleaning Otter DB State...]

:: Delete data chunks
if exist "./Index/bin/" del /Q ".\Index\bin\*.bin"
if exist "./OFS/bin/" del /Q ".\OFS\bin\*.bin"
if exist "./StorageManager/InsertionQueue/bin/" del /Q ".\StorageManager\InsertionQueue\bin\*.bin"
if exist "./StorageManager/LRUCache/config/" del /Q ".\StorageManager\LRUCache\config\*.conf"
if exist "./StorageManager/PersistenceManager/config/" del /Q ".\StorageManager\PersistenceManager\config\*.conf"
@REM if exist "./TransactionManager/RecoveryManager/bin" del /Q ".\TransactionManager\RecoveryManager\bin\*.bin"

echo [Clean Complete]