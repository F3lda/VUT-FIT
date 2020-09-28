###!/bin/bash

# get tests dir and set working dir to makefile dir
TESTS_DIR=$(dirname "${BASH_SOURCE[0]}");
if [[ $TESTS_DIR == "." ]]; then
	TESTS_DIR="./$(basename "$(pwd)")/";
	cd ..;
else
	TESTS_DIR="$TESTS_DIR/";
fi

echo $(pwd)

# make project
make

# TESTS
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

for dir in tests/test_*; do
    PARAM=`cat $dir/param`
	./main $PARAM >$dir/prog_stdout

    diff $dir/prog_stdout $dir/stdout >/dev/null
    if [ $? -eq 0 ] ; then
        echo "TEST $dir"
		echo -e "\t${GREEN} OK${NC}"
    else
        echo "TEST $dir"
		echo -e "\t${RED} FAILED${NC}"
		echo $PARAM
		./main $PARAM
    fi
done

#diff ou

# clean
make clean