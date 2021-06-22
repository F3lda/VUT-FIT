###!/bin/bash


# get tests dir and set working dir to makefile dir
TESTS_DIR=$(dirname "${BASH_SOURCE[0]}");
if [[ $TESTS_DIR == "." ]]; then
	TESTS_DIR="./$(basename "$(pwd)")/";
	cd ..;
else
	TESTS_DIR="$TESTS_DIR/";
fi

pwd

# make project
#make

# TESTS
echo ""
echo "php7.4 parse.php < ./tests/test_files/basic.IPPcode21"
php7.4 parse.php < ./tests/test_files/basic.IPPcode21


echo $?


# clean
#make clean
