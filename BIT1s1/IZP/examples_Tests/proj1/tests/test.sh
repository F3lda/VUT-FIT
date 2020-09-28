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
echo "./proj1  < seznam.txt"
./proj1  < seznam.txt

echo ""
echo "./proj1 12 <seznam.txt"
./proj1 12 <seznam.txt

echo ""
echo "./proj1 686 <seznam.txt"
./proj1 686 <seznam.txt

echo ""
echo "./proj1 38 <seznam.txt"
./proj1 38 <seznam.txt


echo ""
echo "./proj1 111 <seznam.txt"
./proj1 111 <seznam.txt

# clean
make clean