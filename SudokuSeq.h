//
// Created by eleonora on 27/05/20.
//

#ifndef SPM_SUDOKUSEQ_H
#define SPM_SUDOKUSEQ_H

#include "Sudoku.hpp"


class SudokuSeq : public Sudoku {
public:
    int fix_valid_values(configuration &conf);

    int fix_unique_values(configuration &conf);

    void solve(bool printGrid = false);

private:

    std::vector<std::pair<int16_t, int16_t>> check_rows1(int16_t value, configuration &conf);

    std::vector<std::pair<int16_t, int16_t>> check_columns1(int16_t value, configuration &conf);

    std::vector<std::pair<int16_t, int16_t>> check_subgrids1(int16_t value, configuration &conf);

    int one_step(configuration &conf);

    std::vector<int16_t> brute_fix(configuration &conf);
};


int SudokuSeq::fix_valid_values(configuration &conf) {
    int result = 0;
    int complete = 0;
    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM; ++j) {
            if (conf.grid[i][j] == 0) {
                complete ++;
                int i_subgrid = (i / DIM_SUBGRID)*3;
                int j_subgrid = j / DIM_SUBGRID;
                int pos = 0;
                int found = 0;
                //TODO:vedi se si può vettorizzare, conviene su 9 elementi?
                //controllare elemneti disponibili deve diventare una funzione
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
                    fix_value(pos + 1, i, j, conf);
                    result = 1;
                }
                //no possible values, break
                if (found == 0) {
                    result = -1;
                }
            }
            if (result == -1){
                break;
            }
        }
        if (result == -1){
            break;
        }
    }
    if (complete == 0)
        result = -2;
    return result;
}

std::vector<std::pair<int16_t, int16_t>> SudokuSeq::check_rows1(int16_t value, configuration &conf) {
    std::vector<std::pair<int16_t, int16_t>> tofixelem;
    int16_t index = value - 1;
    for (int row = 0; row < DIM; ++row) {
        if (conf.rows_values[row][index]) {
            int i_subgrid = (row / DIM_SUBGRID) * 3;
            int total_found = 0;
            int pos = 0;
            for (int col = 0; col < DIM; ++col) {
                int j_subgrid = col / DIM_SUBGRID;
                if (conf.grid[row][col] == 0 && conf.cols_values[col][index] &&
                    conf.grids_values[i_subgrid + j_subgrid][index]) {
                    total_found++;
                    pos = col;
                }
            }
            if (total_found == 0) {
                tofixelem.emplace_back(std::make_pair(-1, -1));
                break;
            }
            if (total_found == 1) {
                tofixelem.emplace_back(std::make_pair(row, pos));
            }
        }
    }
    return tofixelem;
}

std::vector<std::pair<int16_t, int16_t>> SudokuSeq::check_columns1(int16_t value, configuration &conf) {
    std::vector<std::pair<int16_t, int16_t>> tofixelem;
    int16_t index = value - 1;
    for (int col = 0; col < DIM; ++col) {
        if (conf.cols_values[col][index]) {
            int j_subgrid = col / DIM_SUBGRID;
            int total_found = 0;
            int pos = 0;
            for (int row = 0; row < DIM; ++row) {
                int i_subgrid = (row / DIM_SUBGRID) * 3;
                if (conf.grid[row][col] == 0 && conf.rows_values[row][index] &&
                    conf.grids_values[i_subgrid + j_subgrid][index]) {
                    total_found++;
                    pos = row;
                }
            }
            if (total_found == 0) {
                tofixelem.emplace_back(std::make_pair(-1, -1));
                break;
            }
            if (total_found == 1) {
                tofixelem.emplace_back(std::make_pair(pos, col));
            }
        }
    }
    return tofixelem;
}

std::vector<std::pair<int16_t, int16_t>> SudokuSeq::check_subgrids1(int16_t value, configuration &conf) {
    std::vector<std::pair<int16_t, int16_t>> tofixelem;
    int16_t index = value - 1;
    for (int subgrid = 0; subgrid < DIM; ++subgrid) {
        if (conf.grids_values[subgrid][index]) {
            int total_found = 0;
            int16_t pos_r = 0;
            int16_t pos_c = 0;
            for (int row = 0; row < DIM_SUBGRID; ++row) {
                int offset_row = (subgrid / 3) * 3;
                for (int col = 0; col < DIM_SUBGRID; ++col) {
                    int offset_column = (subgrid % 3) * 3;
                    if (conf.grid[row+offset_row][col+offset_column] == 0 && conf.cols_values[col+offset_column][index] &&
                        conf.rows_values[row+offset_row][index]) {
                        total_found++;
                        pos_r = row+offset_row;
                        pos_c = col+offset_column;
                    }
                }
            }
            if (total_found == 0) {
                tofixelem.emplace_back(std::make_pair(-1, -1));
                break;
            }
            if (total_found == 1) {
                tofixelem.emplace_back(std::make_pair(pos_r, pos_c));
            }
        }
    }
    return tofixelem;
}


int SudokuSeq::fix_unique_values(configuration &conf) {
    int result = 0;
    for (uint16_t value = 1; value <= DIM; ++value) {
        std::vector<std::pair<int16_t, int16_t>> fix_from_rows = check_rows1(value, conf);
        for (auto elem : fix_from_rows) {
            if (elem.first >= 0) {
                fix_value(value,elem.first,elem.second, conf);
                result++;
            } else {
                result = -1;
                break;
            }
        }

        std::vector<std::pair<int16_t, int16_t>> fix_from_columns = check_columns1(value, conf);
        for (auto elem : fix_from_columns) {
            if (elem.first >= 0 && result >=0) {
                fix_value(value,elem.first,elem.second, conf);
                result++;
            } else {
                result = -1;
                break;
            }
        }

        std::vector<std::pair<int16_t, int16_t>> fix_from_subgrid = check_subgrids1(value, conf);
        for (auto elem : fix_from_subgrid) {
            if (elem.first >= 0 && result  >=0) {
                fix_value(value,elem.first,elem.second, conf);
                result++;
            } else {
                result = -1;
                break;
            }
        }
    }
    return result;
}

std::vector<int16_t> SudokuSeq::brute_fix(configuration &conf) {
    std::vector<int16_t> result;
    int row = 0;
    int col = 0;
    int min = 10;
    std::vector<int16_t> availables(DIM, 0);
    for (int i = 0; i < DIM; ++i){
        int i_subgrid = (i / DIM_SUBGRID)*3;
        for (int j = 0; j< DIM; ++j){
            int j_subgrid = j / DIM_SUBGRID;
            if ( conf.grid[i][j]==0){
                for (int v = 0; v < DIM; ++v) {
                    availables[v] = conf.rows_values[i][v] && conf.cols_values[j][v] &&
                                    conf.grids_values[i_subgrid + j_subgrid][v];

                }
                int sum = std::accumulate(availables.begin(),availables.end(),0);
                if (sum < min ) {
                    min = sum;
                    row = i;col =j;
                }
            }

            if (min == 2){
                break;
            }
        }
        if (min == 2){
            break;
        }
    }
    result.push_back(row);
    result.push_back(col);
    for (int v = 0; v < DIM; ++v){
        if (availables[v]){
            result.push_back(v+1);
        }
    }

    return result;
}

int SudokuSeq::one_step(configuration &conf){
    int new_fixed = 1;
    while (new_fixed > 0) {
        new_fixed = fix_valid_values(conf);
        print(conf);
        std::cout << "after valid------------------" << std::endl;
        if(new_fixed >= 0){
            int to_fix_for_unicity = fix_unique_values(conf);
            new_fixed = (to_fix_for_unicity == -1)? -1:std::max(new_fixed,to_fix_for_unicity);
        }
        print(conf);
        std::cout << "after unicity------------------" << std::endl;
    }
    if (new_fixed == -2){
        starting_conf = conf;
        std::cout << "THE END" << std::endl;;
        print(starting_conf);
        new_fixed = 1;
    }

    //TODO: sostituire il primo con elemento di minima scelta
    if (new_fixed == 0){
        std::vector<int16_t> min_choices = brute_fix(conf);
        for(int choice = 2; choice < min_choices.size(); ++choice){
            configuration new_conf(conf);
            fix_value(min_choices[choice], min_choices[0], min_choices[1], new_conf);
            new_fixed = one_step(new_conf);
            print(new_conf);
            std::cout << "after brute fix------------------" << std::endl;
            if (new_fixed >= 0){
                break;
            }
        }
    }

   return new_fixed;

}

void SudokuSeq::solve(bool printGrid) {
    one_step (starting_conf);
    std::cout << "THE END" << std::endl;;
    print(starting_conf);
}



#endif //SPM_SUDOKUSEQ_H
