#!/bin/bash
# Компиляция с указанием путей для macOS (Apple Silicon)
clang++ -std=c++11 main.cpp \
-I/opt/homebrew/include \
-L/opt/homebrew/lib \
-lglfw -framework OpenGL \
-o texture_demo

# Если компиляция успешна — запуск
if [ $? -eq 0 ]; then
    ./texture_demo
else
    echo "Compilation failed"
fi