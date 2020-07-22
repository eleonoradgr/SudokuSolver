//
// Created by eleonora on 27/05/20.
//

#ifndef SPM_SUDOKU_H
#define SPM_SUDOKU_H

#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include "utils/utils_.hpp"

#define DIM 9
#define DIM_SUBGRID 3


struct configuration{
    std::vector<std::vector<int16_t>> grid;
    std::vector<std::vector<int16_t>> rows_values;
    std::vector<std::vector<int16_t>> cols_values;
    std::vector<std::vector<int16_t>> grids_values;
};

class Sudoku {
public:
    explicit Sudoku() {

    };

    int init(std::string &string);

    static void print(configuration &conf);

    static int fix_value(int16_t value, int i, int j, configuration &conf);

    configuration starting_conf;
    configuration solution;
};

int Sudoku::init(std::string &sudoku_string) {
    starting_conf.grid = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,0));
    starting_conf.rows_values = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,1));
    starting_conf.cols_values = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,1));
    starting_conf.grids_values = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,1));

    if (sudoku_string.size() < DIM*DIM){
        std::cout << "Initialization failed, not valid string to build sudoku." << std::endl;
        return -1;
    }

    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM; ++j) {
            int elem = (int)sudoku_string[i*DIM +j] -48;
            if ( elem >  0) {
                starting_conf.grid[i][j] = elem;
                fix_value( starting_conf.grid[i][j], i,j, starting_conf);
            }
        }
    }
    return 0;
}

void Sudoku::print( configuration &conf) {
    for (auto elem : conf.grid) {
        for (auto value : elem) {
            std::string to_print;
            if (value == 0){
                to_print = std::string(" /");
            }else{
                to_print = (value > 0) ? std::string(" ")+std::to_string(value) : std::to_string(value);
            }
            std::cout << " " << to_print << " ";
        }
        std::cout << std::endl;
    }
}

int Sudoku::fix_value(int16_t value,int i, int j, configuration &conf){
    int i_subgrid = (i / DIM_SUBGRID) * DIM_SUBGRID;
    int j_subgrid = j / DIM_SUBGRID;
    int16_t index = value-1;
    int result = 0;
    if (conf.rows_values[i][index] && conf.cols_values[j][index] &&
        conf.grids_values[i_subgrid + j_subgrid][index]){
        conf.grid[i][j] = value ;

        //exclude value for the choices of all the elements in the row;
        conf.rows_values[i][index] = 0;

        //exclude values for all the choices of all the elements in the column;
        conf.cols_values[j][index] = 0;

        //exclude values for all the choices of all the elements in the 3x3 grid;
        conf.grids_values[i_subgrid+j_subgrid][index] = 0;

        result = 1;
    }
    return result;
}


#endif //SPM_SUDOKU_H
