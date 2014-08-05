#!/bin/sh

# cd to get around . returned if script is in working dir
base_dir=$(cd $(dirname "$0")/..; pwd)
cd $base_dir

cd curses-shell
make uninstall

cd $base_dir
make uninstall
