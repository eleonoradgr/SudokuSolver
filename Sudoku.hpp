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
#define DEFAULT_VALUE 0
#define DELIMITER ","

struct cell_choices {
    std::vector<bool> values = std::vector<bool>(DIM, true);
    bool fixed = false;
};
struct configuration{
    std::vector<std::vector<int16_t>> grid;
    std::vector<std::vector<int16_t>> rows_values;
    std::vector<std::vector<int16_t>> cols_values;
    std::vector<std::vector<int16_t>> grids_values;
};

class Sudoku {
public:
    explicit Sudoku() {};

    void init(std::string &filename);

    void print(configuration &conf);

    void fix_value(int16_t value,int i, int j, configuration &conf);

    virtual int fix_valid_values(configuration &conf) = 0;
    virtual int fix_unique_values(configuration &conf) = 0;
    virtual void solve(bool printGrid = false) = 0;


protected:
    configuration starting_conf;
};

void Sudoku::init(std::string &filename) {

    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::int16_t> splitted(split(line, DELIMITER));
            starting_conf.grid.push_back(splitted);
        }
    } else {
        std::cout << "Initialization failed, not valid file." << std::endl;
    }

    starting_conf.rows_values = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,1));
    starting_conf.cols_values = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,1));
    starting_conf.grids_values = std::vector<std::vector<int16_t>>(DIM,std::vector<int16_t>(DIM,1));

    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM; ++j) {
            if (starting_conf.grid[i][j]< 0) {
                fix_value(- starting_conf.grid[i][j], i,j, starting_conf);
            }
        }
    }
}

void Sudoku::print( configuration &conf) {
    /*for (auto elem : grid) {
        for (auto value : elem) {
            std::string to_print = (value == 0) ? std::string("/") : std::to_string(value);
            std::cout << " " << to_print << " ";
        }
        std::cout << std::endl;
    }
    for (auto elem : choices) {
        for (auto elem2 : elem) {
            for (auto elem3 : elem2.values) {
                std::cout << to_string(elem3) ;
            }
            std::cout << " nc: "<< std::endl;
        }
        std::cout << std::endl;
    }*/
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

void Sudoku::fix_value(int16_t value,int i, int j, configuration &conf){
    conf.grid[i][j] = value ;
    int16_t index = value-1;

    //exclude value for the choices of all the elements in the row;
    conf.rows_values[i][index] = 0;

    //exclude values for all the choices of all the elements in the column;
    conf.cols_values[j][index] = 0;

    //exclude values for all the choices of all the elements in the 3x3 grid;
    int i_subgrid = (i / DIM_SUBGRID)*3;
    //int j_subgrid = (j / DIM_SUBGRID)*3;
    int j_subgrid = j / DIM_SUBGRID;
    conf.grids_values[i_subgrid+j_subgrid][index] = 0;
}


#endif //SPM_SUDOKU_H
