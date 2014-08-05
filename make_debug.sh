#!/bin/sh

# cd to get around . returned if script is in same dir
base_dir=$(cd $(dirname "$0"); pwd)
cd $base_dir
output_dir="${base_dir}/output/debug"

mkdir output_dir

./make_curses_shell.sh $output_dir

cd curses-shell
make install
cd $base_dir

rm CMakeCache.txt
cmake -DCMAKE_INSTALL_PREFIX:PATH=${output_dir} .
make install
