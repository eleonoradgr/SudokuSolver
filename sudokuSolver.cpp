//
// Created by eleonora on 27/05/20.
//
#include "SudokuSeq.h"

int main(int argc, char* argv[]){
    SudokuSeq prova;
    std::string filename("../SudokuSolver/tests/sudokuvh.txt");

    prova.init(filename);
    std::cout << "------------------"<<std::endl;
    prova.solve();
    return 0;
}