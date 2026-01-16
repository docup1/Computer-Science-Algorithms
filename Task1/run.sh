#!/bin/bash

clang++ -std=c++11 main.cpp \
-I/opt/homebrew/include \
-L/opt/homebrew/lib \
-lglfw \
-framework OpenGL \
-o star_demo

./star_demo