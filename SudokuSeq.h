//
// Created by eleonora on 27/05/20.
//

#ifndef SPM_SUDOKUSEQ_H
#define SPM_SUDOKUSEQ_H

#include <deque>
#include <stack>
#include "Sudoku.hpp"
#include "utils/utimer.hpp"

#define LIMIT 180

std::vector<std::pair<int16_t, int16_t>> check_rows1(int16_t value, configuration &conf) {
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

std::vector<std::pair<int16_t, int16_t>> check_columns1(int16_t value, configuration &conf) {
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

std::vector<std::pair<int16_t, int16_t>> check_subgrids1(int16_t value, configuration &conf) {
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
                    if (conf.grid[row + offset_row][col + offset_column] == 0 &&
                        conf.cols_values[col + offset_column][index] &&
                        conf.rows_values[row + offset_row][index]) {
                        total_found++;
                        pos_r = row + offset_row;
                        pos_c = col + offset_column;
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

int fix_unique_values(configuration &conf) {
    int result = 0;
    for (uint16_t value = 1; value <= DIM; ++value) {
        std::vector<std::pair<int16_t, int16_t>> fix_from_rows = check_rows1(value, conf);
        for (auto elem : fix_from_rows) {
            if (elem.first >= 0) {
                Sudoku::fix_value(value, elem.first, elem.second, conf);
                result++;
            } else {
                result = -1;
                break;
            }
        }

        std::vector<std::pair<int16_t, int16_t>> fix_from_columns = check_columns1(value, conf);
        for (auto elem : fix_from_columns) {
            if (elem.first >= 0 && result >= 0) {
                Sudoku::fix_value(value, elem.first, elem.second, conf);
                result++;
            } else {
                result = -1;
                break;
            }
        }

        std::vector<std::pair<int16_t, int16_t>> fix_from_subgrid = check_subgrids1(value, conf);
        for (auto elem : fix_from_subgrid) {
            if (elem.first >= 0 && result >= 0) {
                Sudoku::fix_value(value, elem.first, elem.second, conf);
                result++;
            } else {
                result = -1;
                break;
            }
        }
    }
    return result;
}

int filter(configuration &conf) {
    int new_fixed = 1;
    while (new_fixed > 0) {
        new_fixed = fix_valid_values(conf);
        if (new_fixed >= 0) {
            int to_fix_for_unicity = fix_unique_values(conf);
            new_fixed = (to_fix_for_unicity == -1) ? -1 : std::max(new_fixed, to_fix_for_unicity);
        }
    }
    return new_fixed;
}

int BFS_seq(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::deque<configuration *> test_conf;
    test_conf.push_back(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf.front();
        test_conf.pop_front();
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) { // continue to fix values in cells with only one feasible choice
                new_fixed = fix_valid_values(*conf);
            }
            if (new_fixed == -1) { // invalid configuration found
                delete conf;
                break;
            }
            if (new_fixed == -2) { // solution found
                found++;
                solution_conf=*conf;
                delete conf;
                while(!test_conf.empty()){
                    configuration *conf = test_conf.front();
                    test_conf.pop_front();
                    delete conf;
                }
            }

            if (new_fixed == 0) { // generate new configurations and insert it in the queue
                    std::vector<int16_t> min_choices = brute_fix(*conf);
                    for (int choice = 2; choice < min_choices.size(); ++choice) {
                        configuration *new_conf = new configuration(*conf);
                        Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                        test_conf.push_back(new_conf);
                    }
                    break;
            }
        }
    }
    return 1;
}

int mix_seq(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::deque<configuration *> test_conf;
    test_conf.push_back(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf.front();
        test_conf.pop_front();
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) { // continue to fix values in cells with only one feasible choice
                new_fixed = fix_valid_values(*conf);
            }
            if (new_fixed == -1) { // invalid configuration found
                delete conf;
                break;
            }
            if (new_fixed == -2) { // solution found
                found++;
                solution_conf=*conf;
                delete conf;
                while(!test_conf.empty()){
                    configuration *conf = test_conf.front();
                    test_conf.pop_front();
                    delete conf;
                }
            }

            if (new_fixed == 0) { // generate new configurations and insert it in the queue axcept the first one
                // which continue to be investigated.

                std::vector<int16_t> min_choices = brute_fix(*conf);
                for (int choice = 3; choice < min_choices.size(); ++choice) {
                    configuration *new_conf = new configuration(*conf);
                    Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                    test_conf.push_back(new_conf);
                }
                configuration *new_conf = new configuration(*conf);;
                Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *new_conf);
                delete conf;
                conf = new_conf;
            }
        }
    }
    return 1;
}

int mix_seq2(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::deque<configuration *> test_conf;
    test_conf.push_back(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf.front();
        test_conf.pop_front();
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) { // continue to fix values in cells with only one feasible choice
                new_fixed = fix_valid_values(*conf);
            }
            if (new_fixed == -1) { // invalid configuration found
                delete conf;
                break;
            }
            if (new_fixed == -2) { // solution found
                found++;
                solution_conf=*conf;
                delete conf;
                while(!test_conf.empty()){
                    configuration *conf = test_conf.front();
                    test_conf.pop_front();
                    delete conf;
                }
            }

            if (new_fixed == 0) { // generate new configurations and insert it in the queue
                int prova = test_conf.size();
                if (prova > LIMIT) {
                    new_fixed = rec_sol_seq(*conf);
                    if (new_fixed == -1) {
                        delete conf;
                        break;
                    }
                    if (new_fixed == -2) {
                        found++;
                        solution_conf=*conf;
                        delete conf;
                        while(!test_conf.empty()){
                            configuration *conf = test_conf.front();
                            test_conf.pop_front();
                            delete conf;
                        }
                    }
                } else {
                    std::vector<int16_t> min_choices = brute_fix(*conf);
                    for (int choice = 3; choice < min_choices.size(); ++choice) {
                        configuration *new_conf = new configuration(*conf);
                        Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                        test_conf.push_back(new_conf);
                    }
                    configuration *new_conf = new configuration(*conf);;
                    Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *new_conf);
                    delete conf;
                    conf = new_conf;
                }
            }
        }
    }
    return 1;
}

int DFS_seq(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::stack<configuration *> test_conf;
    test_conf.push(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf.top();
        test_conf.pop();
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) {
                new_fixed = fix_valid_values(*conf);
                /*if (new_fixed >= 0) {
                    int to_fix_for_unicity = fix_unique_values(conf);
                    new_fixed = (to_fix_for_unicity == -1) ? -1 : std::max(new_fixed, to_fix_for_unicity);
                }*/
            }
            if (new_fixed == -1) {
                delete conf;
                break;
            }
            if (new_fixed == -2) {
                found++;
                solution_conf=*conf;
                delete conf;
                while(!test_conf.empty()){
                    delete test_conf.top();
                    test_conf.pop();
                }
            }
            if (new_fixed == 0) {
                    std::vector<int16_t> min_choices = brute_fix(*conf);
                    for (int choice = 3; choice < min_choices.size(); ++choice) {
                        configuration *new_conf = new configuration(*conf);
                        Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                        test_conf.push(new_conf);
                    }
                    configuration *new_conf = new configuration(*conf);;
                    Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *new_conf);
                    delete conf;
                    conf = new_conf;
            }
        }
    }
    return 1;
}


void seq_solve(Sudoku &sudoku) {
        DFS_seq(sudoku.starting_conf,sudoku.solution);
}


#endif //SPM_SUDOKUSEQ_H
