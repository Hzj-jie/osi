#!/bin/sh
set -e

cd "$(dirname "$0")"

g++ -std=c++20 -O2 importer_gen.cpp -o importer_gen
./importer_gen < ../definition.txt > ../importer.definition.hpp
rm -f importer_gen

g++ -std=c++20 -O2 builder_gen.cpp -o builder_gen
./builder_gen < ../definition.txt > ../builders.implementation.hpp
rm -f builder_gen
