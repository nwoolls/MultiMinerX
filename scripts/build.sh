#!/bin/sh

# cd to get around . returned if script is in working dir
base_dir=$(cd $(dirname "$0")/..; pwd)
cd $base_dir

# fetch sub-projects
# revert any changes
cd curses-shell
git checkout .
cd $base_dir
# first init the directory, in case it does not exist
git submodule update --remote --init
# next merge in any remote changes, in case it does exist
git submodule update --remote --merge

# customize sub-projects
cp branding-cust.cmake curses-shell

# determine output directory from args
output_dir="${base_dir}/output/release"
debug=false
install=false

shopt -s nocasematch
if [[ "$1" == "DEBUG" ]]
then
	output_dir="${base_dir}/output/debug"
	debug=true
fi

if [[ "$2" == "install" ]]
then
	install=true
fi

mkdir output_dir

shell_make_args="-DENABLE_GENERIC=ON -DENABLE_IRC=OFF -DENABLE_ASPELL=OFF -DENABLE_RELAY=OFF -DENABLE_XFER=OFF"
app_make_args=""

if [[ $debug == true ]]
then
	shell_make_args="${shell_make_args} -DCMAKE_BUILD_TYPE=Debug"
	app_make_args="${app_make_args} -DCMAKE_BUILD_TYPE=Debug"
fi

if [[ $install != true ]]
then
	shell_make_args="${shell_make_args} -DCMAKE_INSTALL_PREFIX:PATH=${output_dir}"
	app_make_args="${app_make_args} -DCMAKE_INSTALL_PREFIX:PATH=${output_dir}"
fi

if [ "$(uname)" == "Darwin" ]; then
	# Guile plugin causes app to crash on /quit on OS X
	shell_make_args="${shell_make_args} -DENABLE_GUILE=OFF"
fi

cd curses-shell
rm -f CMakeCache.txt
cmake $shell_make_args .
make install
cd $base_dir

rm -f CMakeCache.txt
cmake $app_make_args .
make install
rm -rf output_dir
