#!/bin/sh

# clean up sub-projects
rm -rf curses-shell

# fetch sub-projects
git submodule update --remote --merge

# customize sub-projects
cp branding-cust.cmake curses-shell

# build sub-projects
./make_curses_shell.sh

# remove project artifacts
rm -f CMakeCache.text

