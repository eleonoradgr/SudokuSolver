//
// Created by eleonora on 27/05/20.
//

#ifndef SPM_SUDOKU_H
#define SPM_SUDOKU_H

#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include "utils/utils_.hpp"

#define DIM 9
#define DEFAULT_VALUE 0
#define DELIMITER ","

class Sudoku {
public:
    explicit Sudoku(){};

    void init(std::string &filename);

    void print();

    virtual void solve( bool printGrid = false) = 0;

private:
    std::vector<std::vector<uint16_t>> grid;
    std::vector<std::vector<std::vector<bool>>> choices;
};

void Sudoku::init(std::string &filename) {

    std::ifstream file(filename);
    if(file.is_open()){
        grid = std::vector<std::vector<uint16_t>>(DIM);
        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::uint16_t> splitted(split(line, DELIMITER));
            grid.push_back(splitted);
        }
    }else{
        std::cout << "Initialization failed, not valid file." << std::endl;
    }


}

void Sudoku::print(){
    for (auto elem : grid){
        for (auto value : elem){
            std::string to_print = (value == 0)? std::string("/") : std::to_string(value);
            std::cout<< " " << to_print << " " ;
        }
        std::cout<< std::endl;
    }
}



#endif //SPM_SUDOKU_H
