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
echo "./IFJ2020  < ./tests/test_files/faktorial.ifj20"
./IFJ2020  < ./tests/test_files/faktorial.ifj20


echo ""
echo "./IFJ2020  < ./tests/test_files/retezce.ifj20"
./IFJ2020  < ./tests/test_files/retezce.ifj20


echo ""
echo "./IFJ2020  < ./tests/test_files/scitani.ifj20"
./IFJ2020  < ./tests/test_files/scitani.ifj20


echo ""
echo "./IFJ2020  < ./tests/test_files/fakt_rek.ifj20"
./IFJ2020  < ./tests/test_files/fakt_rek.ifj20


echo ""
echo "./IFJ2020  < ./tests/test_files/test1.ifj20"
./IFJ2020  < ./tests/test_files/test1.ifj20



# clean
make clean
