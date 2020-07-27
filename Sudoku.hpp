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
    std::cout << " --------------------------------- "<< std::endl;
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
    std::cout << " --------------------------------- "<< std::endl;
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


int fix_valid_values(configuration &conf) {
    int result = 0;
    int complete = 0;
    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM; ++j) {
            if (conf.grid[i][j] == 0) {
                complete++;
                int i_subgrid = (i / DIM_SUBGRID) * 3;
                int j_subgrid = j / DIM_SUBGRID;
                int pos = 0;
                int found = 0;

                //check the possible values remaing for that cell
                std::vector<int16_t> availables(DIM, 0);
                for (int v = 0; v < DIM; ++v) {
                    availables[v] = conf.rows_values[i][v] && conf.cols_values[j][v] &&
                                    conf.grids_values[i_subgrid + j_subgrid][v];
                    if (availables[v]) {
                        pos = v;
                        found++;
                    }
                }
                //only one possible value, fix it
                if (found == 1) {
                    conf.grid[i][j] = (pos + 1);
                    Sudoku::fix_value(pos + 1, i, j, conf);
                    result = 1;
                }
                //no possible values, break
                if (found == 0) {
                    result = -1;
                }
            }
            if (result == -1) {
                break;
            }
        }
        if (result == -1) {
            break;
        }
    }
    if (complete == 0)
        result = -2;
    return result;
}

std::vector<int16_t> brute_fix(configuration &conf) {
    std::vector<int16_t> result;
    int row = 0;
    int col = 0;
    int min = 10;
    std::vector<int16_t> availables(DIM, 0);
    for (int i = 0; i < DIM; ++i) {
        int i_subgrid = (i / DIM_SUBGRID) * 3;
        for (int j = 0; j < DIM; ++j) {
            int j_subgrid = j / DIM_SUBGRID;
            if (conf.grid[i][j] == 0) {
                std::vector<int16_t> availables_tmp(DIM, 0);

                for (int v = 0; v < DIM; ++v) {
                    availables_tmp[v] = conf.rows_values[i][v] && conf.cols_values[j][v] &&
                                        conf.grids_values[i_subgrid + j_subgrid][v];

                }
                int sum = std::accumulate(availables_tmp.begin(), availables_tmp.end(), 0);
                if (sum < min) {
                    min = sum;
                    row = i;
                    col = j;
                    availables = availables_tmp;
                }
            }

            if (min == 2) {
                break;
            }
        }
        if (min == 2) {
            break;
        }
    }
    result.push_back(row);
    result.push_back(col);
    for (int v = 0; v < DIM; ++v) {
        if (availables[v]) {
            result.push_back(v + 1);
        }
    }

    return result;
}

int rec_sol_seq(configuration &conf) {
    int new_fixed = 1;
    while (new_fixed > 0) {
        new_fixed = fix_valid_values(conf);
    }

    if (new_fixed == 0) {
        new_fixed = -1;
        std::vector<int16_t> min_choices = brute_fix(conf);
        for (int choice = 2; choice < min_choices.size(); ++choice) {
            configuration new_conf(conf);
            Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], new_conf);
            new_fixed = rec_sol_seq(new_conf);
            if (new_fixed == -2) {
                conf = new_conf;
                break;
            }
        }
    }
    return new_fixed;
}



#endif //SPM_SUDOKU_H
