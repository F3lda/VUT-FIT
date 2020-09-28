###!/bin/bash

# get tests dir and set working dir to makefile dir
TESTS_DIR=$(dirname "${BASH_SOURCE[0]}");
if [[ $TESTS_DIR == "." ]]; then
	TESTS_DIR="./$(basename "$(pwd)")/";
	cd ..;
else
	TESTS_DIR="$TESTS_DIR/";
fi



# make project
make

# TESTS
echo ""
echo "./proj2 12 47 1e-2"
./proj2 12 47 1e-2

echo ""
echo "./proj2 12 47 1e-7"
./proj2 12 47 1e-7

echo ""
echo "./proj2 24 330 1e-7"
./proj2 24 330 1e-7

echo ""
echo "./proj2 24 330 1e-20"
./proj2 24 330 1e-20

echo ""
echo "./proj2 inf 5 .5"
./proj2 inf 5 .5

echo ""
echo "./proj2 -33 5 .1"
./proj2 -33 5 .1

# clean
make clean