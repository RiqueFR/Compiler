#!/bin/bash
EXEC=compiler
TEST_FOLDER=tests
TEST_OUT_FOLDER=tests/out

make
rm $TEST_OUT_FOLDER/*
find $TEST_FOLDER/ -maxdepth 1 -type f -printf "%f\n" | while read -r file; do
	echo "Generating file: $file"
	./$EXEC < $TEST_FOLDER/"$file" > $TEST_OUT_FOLDER/"$file"
done
