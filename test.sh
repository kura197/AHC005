#!/bin/sh

IN=$1
TESTER=$2
OUT=./out/$3.txt

VIS=./tools/target/release/vis

$TESTER < $IN > $OUT

$VIS $IN $OUT

#rm $OUT
