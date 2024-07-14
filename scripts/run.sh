#!/bin/bash

if [ "$1" = "release" ]
then
    ./build/release/bin/walkson
else
    ./build/debug/bin/walkson
fi