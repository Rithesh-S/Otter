@echo off
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set "ESC=%%b"

@echo off
echo %ESC%[33mCompiling...%ESC%[0m

g++ -g ./Test/unit_tests.cpp ./StorageManager/InsertionQueue/InsertionQueue.cpp ./StorageManager/LRUCache/LRUCache.cpp ./StorageManager/StorageManager.cpp ./OFS/BufferPool/BufferPool.cpp ./OFS/DataNode/DataNode.cpp ./OFS/Page/Page.cpp ./Index/BTree/Btree.cpp ./Index/LRU/LRUTree.cpp ./StorageManager/PersistenceManager/PersistenceManager.cpp -o ./Test/output/db_bench.exe -std=c++17

if %errorlevel% neq 0 (
    echo %ESC%[31mBuild Failed...%ESC%[0m
    exit /b %errorlevel%
)

echo %ESC%[32mComplied Successfully...%ESC%[0m