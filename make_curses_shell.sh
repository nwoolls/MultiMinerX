#!/bin/sh

# cd to get around . returned if script is in same dir
base_dir=$(cd $(dirname "$0"); pwd)
cd $base_dir

make_args="-DENABLE_GENERIC=ON -DENABLE_IRC=OFF -DENABLE_ASPELL=OFF -DENABLE_RELAY=OFF -DENABLE_XFER=OFF"

if [ ! -z "$1" ]
then
	make_args="-DCMAKE_INSTALL_PREFIX:PATH=${1} ${make_args}"
fi

cd curses-shell
rm CMakeCache.txt
cmake $make_args .
cd $base_dir
