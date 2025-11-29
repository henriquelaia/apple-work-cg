#!/bin/bash

# Criar diretório de build se não existir
if [ ! -d "build" ]; then
  mkdir build
fi

# Ir para o diretório de build
cd build

cmake ..

make

if [ $? -eq 0 ]; then
    ./app
else
    echo "Build failed."
fi
