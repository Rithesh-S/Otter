@echo off
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set "ESC=%%b"

@echo off
echo %ESC%[33mCompiling...%ESC%[0m

g++ -g ./unit_tests.cpp ./CLIComponent/CLInterface.cpp ./OFS/DataNode/DataNode.cpp ./OFS/Buffer/Buffer.cpp ./StorageManager/StorageManager.cpp ./Index/BTree/BTree.cpp ./OFS/WAL/WAL.cpp ./StorageManager/LRUCache/LRUCache.cpp ./StorageManager/InsertionQueue/InsertionQueue.cpp ./TransactionManager/Transaction.cpp ./QueryProcessor/Lexer/Lexer.cpp ./QueryProcessor/Parser/Parser.cpp ./QueryProcessor/Executor/Executor.cpp ./Index/LRU/LRUTree.cpp -o ./db_bench.exe -std=c++17

if %errorlevel% neq 0 (
    echo %ESC%[31mBuild Failed...%ESC%[0m
    exit /b %errorlevel%
)

echo %ESC%[32mComplied Successfully...%ESC%[0m