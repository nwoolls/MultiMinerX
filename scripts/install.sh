#!/bin/sh

# cd to get around . returned if script is in working dir
scripts_dir=$(cd $(dirname "$0"); pwd)
cd $scripts_dir

debug=false

shopt -s nocasematch
if [[ "$1" == "DEBUG" ]]
then
	./build.sh debug install
else
	./build.sh release install
fi
