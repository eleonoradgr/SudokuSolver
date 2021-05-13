# SudokuSolver
To run the code it is needed to create a new directory called lib and clone in it the [Fast-
Flow code](https://github.com/fastflow) and then run the script: `/ff/mapping string.sh`.
The command to compile is:

````
g++ -I./utils -I../lib/fastflow -std=c++17 -pthread -O3 -DNDEBUG sudokuSolver.cpp
````

Once the executable has been obtained the parameters needed are the following:
- **filename**:  name of the file that contains starting configuration of Sudoku (mandatory). 
  A starting configuration is a string of 81 digits,it contains the digits of the sudoku from the first row to the last, from left to right.
  For the empty cells the digit used is 0. An example of a valid configuration is:
  
  ````
  400070000000000200000010000030500600008009000000000040120000070000800001600200000
  ````

- **n** : Parallelism degree (mandatory)
- **mod** : a string between all, seq, pt df s, pt mix, ff (default all).
- **repetition** : Number of times the experiment must be repeated (default 1).
- **print** : Boolean for printing the starting configuration and the solution after each execution(default false).
  
  
The script used for experiments is in the file `experiments.sh`. The Sudoku used to test the different implementations are in the file tests.txt.

Additional details can be found in `SudokuSolver_REPORT.pdf`.