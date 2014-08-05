#!/bin/sh

debug=false

shopt -s nocasematch
if [[ "$1" == "DEBUG" ]]
then
	./build.sh debug install
else
	./build.sh release install
fi
