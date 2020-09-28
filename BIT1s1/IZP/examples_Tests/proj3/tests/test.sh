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


SRC="src/"
TESTS="tests/"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

ACT_DIR=`pwd`
DIR=`dirname "${BASH_SOURCE[0]}"`

#$1 path
#$2 parameters
#$3 input
function proj3_test {
    OUTPUT=`./proj3 --$1 $2 "$TESTS"input/$3`
    RET=$?
  
    echo "$OUTPUT" |  diff "$TESTS"output/${3}_${1} - 2>/dev/null
    RET_DIFF=$?
    
    
    echo "./proj3 --$1 $2 "$TESTS"input/${3}"
    if [ $RET -ne 0  ] || [ $RET_DIFF -ne 0 ]; then
        echo -e "\t${RED} FAILED${NC}"
    else
        echo -e "\t${GREEN} OK${NC}"
    fi
}

#1 input
function proj3_test_invalid {

    ./proj3 --test "$TESTS"input/$1 2>/dev/null >/dev/null
    RET=$?
    echo "./proj3 --test "$TESTS"input/$1 2>/dev/null >/dev/null"
    if [ $RET -eq 0  ]; then
        echo -e "\t${RED} FAILED${NC}"
    else
        echo -e "\t${GREEN} OK${NC}"
    fi
}


#$1 path
#$2 parameters
#$3 input
function proj3_test_input_invalid {
    ./proj3 --$1 $2 "$TESTS"input/$3 2>/dev/null >/dev/null
    RET=$?
  
    echo "./proj3 --$1 $2 "$TESTS"input/$3 2>/dev/null >/dev/null"
    if [ $RET -eq 0  ] ; then
        echo -e "\t${RED} FAILED${NC}"
    else
        echo -e "\t${GREEN} OK${NC}"
    fi
}


#$1 path
#$2 parameters
#$3 input
function proj3_test_valgrind {
    echo "" 
    echo "" 
    echo "valgrind ./proj3 --$1 $2 "$TESTS"input/$3 >/dev/null"
    valgrind ./proj3 --$1 $2 "$TESTS"input/$3 >/dev/null
}


gcc -std=c99 -Wall -Wextra -Werror "$SRC"proj3.c -o proj3
if [ $? -ne 0 ]; then
    cd $ACT_DIR
    echo -e "\t${RED} THIS PROJECT CANNOT NOT BE COMPILED${NC}" 
    exit 1
fi


#OUTPUT=`./proj3 --rpath 3 4 "$TESTS"input/in1.txt`
echo ""
echo "TEST OK"
proj3_test "rpath" "3 4" "in1" 
proj3_test "lpath" "3 4" "in1" 
proj3_test "rpath" "6 1" "in2" 
proj3_test "lpath" "6 1" "in2" 

echo ""
echo "TEST FAILED"
proj3_test_invalid "in_invalid.txt"
proj3_test_invalid "in_less.txt"


echo ""
echo "TEST INVALID INPUT"
proj3_test_input_invalid "rdfs"  "3 4" "in1"
proj3_test_input_invalid "rpath" "0 0" "in1"
proj3_test_input_invalid "rpath" "1 dafa" "in1"
proj3_test_input_invalid "rpath" "1 1" "fd"


echo ""
echo "TEST VALGRIND"
proj3_test_valgrind "rpath" "3 4" "in1" 
proj3_test_valgrind "lpath" "3 4" "in1" 
proj3_test_valgrind "rpath" "6 1" "in2" 
proj3_test_valgrind "lpath" "6 1" "in2" 




cd $ACT_DIR

# clean
make clean

