#!/bin/bash
echo "compile commenced at:" $(date)
  em++ -std=c++1z game/src/$1.cpp game/src/Render.cpp game/src/WorldGen.cpp game/src/FastNoise.cpp game/src/View.cpp game/src/Factories.cpp\
  -s WASM=1 -s USE_SDL=2 -O1 \
  --profiling\
  -s EXPORTED_FUNCTIONS="['_main']" \
  -s EXPORTED_RUNTIME_METHODS=["cwrap"] \
  -s USE_SDL_IMAGE=2\
  -s ALLOW_MEMORY_GROWTH=1 --use-preload-plugins\
  -s SDL2_IMAGE_FORMATS='["png"]'\
  -s USE_SDL_TTF=2\
  -s USE_SDL_GFX=2\
  -lSDL \
  -o $1.js\
  -s ASSERTIONS=1\
  -s LLD_REPORT_UNDEFINED
  
  #-s DISABLE_EXCEPTION_CATCHING=0
  #-O3 for minifying