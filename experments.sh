#!/bin/bash

echo "START TESTS"

g++ -I./utils -I../lib/fastflow -std=c++17 -pthread -O3 -DNDEBUG sudokuSolver.cpp -o sudokuSolver

MAX_NUM_THREAD=5
REP=4
FILENAME="tests/tests.txt"

echo " filename = $FILENAME max num thread = $MAX_NUM_THREAD. repetitions = $REP."

./sudokuSolver $FILENAME 1 seq $REP false
sleep 1


for ((i=1;i<=MAX_NUM_THREAD;i++)); do
  ./sudokuSolver $FILENAME $i pt_dfs $REP false
  sleep 1
done

for ((i=1;i<=MAX_NUM_THREAD;i++)); do
  ./sudokuSolver $FILENAME $i pt_mix $REP false
  sleep 1
done

for ((i=1;i<=MAX_NUM_THREAD;i++)); do
  ./sudokuSolver $FILENAME $i ff $REP false
  sleep 1
done

rm sudokuSolver

echo "END TESTS"