#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/TileMapEditor 
else
    ./bin/debug/TileMapEditor 
fi