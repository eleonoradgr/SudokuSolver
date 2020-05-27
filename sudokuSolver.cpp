//
// Created by eleonora on 27/05/20.
//
#include "Sudoku.hpp"

int main(int argc, char* argv[]){
    Sudoku prova;
    std::string filename("../SudokuSolver/tests/sudoku_hard.txt");

    prova.init(filename);
    prova.print();
    return 0;
}