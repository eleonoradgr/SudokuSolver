//
// Created by eleonora on 27/05/20.
//

#ifndef SPM_SUDOKUSEQ_H
#define SPM_SUDOKUSEQ_H

#include <deque>
#include <stack>
#include "Sudoku.hpp"


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

int one_step_seq(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::deque<configuration *> test_conf;
    test_conf.push_back(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf.front();
        test_conf.pop_front();
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) {
                new_fixed = fix_valid_values(*conf);
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
                    configuration *conf = test_conf.front();
                    test_conf.pop_front();
                    delete conf;
                }
            }

            //TODO: sostituire il primo con elemento di minima scelta
            if (new_fixed == 0) {
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
int basta(configuration &conf) {
    int new_fixed = 1;
    while (new_fixed > 0) {
        new_fixed = fix_valid_values(conf);
        /*if (new_fixed >= 0) {
            int to_fix_for_unicity = fix_unique_values(conf);
            new_fixed = (to_fix_for_unicity == -1) ? -1 : std::max(new_fixed, to_fix_for_unicity);
        }*/
    }
    //if (new_fixed == -2) {
    //    new_fixed = 1;
    //}

    //TODO: sostituire il primo con elemento di minima scelta
    if (new_fixed == 0) {
        new_fixed = -1;
        std::vector<int16_t> min_choices = brute_fix(conf);
        for (int choice = 2; choice < min_choices.size(); ++choice) {
            configuration new_conf(conf);
            Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], new_conf);
            new_fixed = basta(new_conf);
            if (new_fixed == -2) {
                conf = new_conf;
                break;
            }
        }
    }
    return new_fixed;
}

int one_step_seq2(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::vector<configuration *> test_conf;
    test_conf.reserve(100 * 1024);
    int index = 0;
    test_conf.push_back(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf[index];
        index++;
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) {
                new_fixed = fix_valid_values(*conf);
            }
            if (new_fixed == -1) {
                delete conf;
                break;
            }
            if (new_fixed == -2) {
                found++;
                solution_conf=*conf;
                delete conf;
                for (int j = index; j< test_conf.size(); ++j){
                    delete test_conf[j];
                }
            }

            //TODO: sostituire il primo con elemento di minima scelta
            if (new_fixed == 0) {
                if (test_conf.size()-index>320){
                    new_fixed = basta(*conf);
                    if (new_fixed == -1) {
                        delete conf;
                        break;
                    }
                    if (new_fixed == -2) {
                        found++;
                        starting_conf = *conf;
                    }
                }else{
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

int one_step_seq3(configuration &starting_conf, configuration &solution_conf) {
    int found = 0;
    configuration *copy = new configuration(starting_conf);
    std::stack<configuration *> test_conf;
    //test_conf.reserve(100 * 1024);
    int index = 0;
    test_conf.push(copy);
    while (!found && !test_conf.empty()) {
        configuration *conf = test_conf.top();
        test_conf.pop();
        index++;
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) {
                new_fixed = fix_valid_values(*conf);
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

            //TODO: sostituire il primo con elemento di minima scelta
            if (new_fixed == 0) {
                if (test_conf.size()>320){
                    new_fixed = basta(*conf);
                    if (new_fixed == -1) {
                        delete conf;
                        break;
                    }
                    if (new_fixed == -2) {
                        found++;
                        starting_conf = *conf;
                    }
                }else{
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
    }
    return 1;
}

configuration *one_step(configuration *conf) {
    std::deque<configuration *> test_conf;
    test_conf.push_back(conf);
    int found = 0;
    while (!test_conf.empty() && !found) {
        configuration *c = test_conf.front();
        test_conf.pop_front();
        while (c != nullptr) {
            int new_fixed = 1;
            while (new_fixed > 0) {
                new_fixed = fix_valid_values(*c);
                /*if (new_fixed >= 0) {
                    int to_fix_for_unicity = fix_unique_values(conf);
                    new_fixed = (to_fix_for_unicity == -1) ? -1 : std::max(new_fixed, to_fix_for_unicity);
                }*/
            }
            if (new_fixed == -2) {
                return c;
            }
            if (new_fixed == -1) {
                c = nullptr;
            }
            if (new_fixed == 0) {
                std::vector<int16_t> min_choices = brute_fix(*c);
                for (int choice = 3; choice < min_choices.size(); ++choice) {
                    configuration new_conf(*conf);
                    Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], new_conf);
                    test_conf.push_back(&new_conf);
                }
                Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *c);
            }
        }

    }
    return nullptr;
}

int one_step_rec(configuration &conf) {
    int new_fixed = 1;
    while (new_fixed > 0) {
        new_fixed = fix_valid_values(conf);
        /*if (new_fixed >= 0) {
            int to_fix_for_unicity = fix_unique_values(conf);
            new_fixed = (to_fix_for_unicity == -1) ? -1 : std::max(new_fixed, to_fix_for_unicity);
        }*/
    }
    if (new_fixed == -2) {
        //starting_conf = conf;
        new_fixed = 1;
    }
    if (new_fixed == 0) {
        std::vector<int16_t> min_choices = brute_fix(conf);
        for (int choice = 2; choice < min_choices.size(); ++choice) {
            configuration new_conf(conf);
            Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], new_conf);
            new_fixed = one_step_rec(new_conf);
            if (new_fixed >= 0) {
                conf = new_conf;
                break;
            }
        }
    }

    return new_fixed;
}

std::vector<int16_t> brute_fix1(configuration &conf) {
    std::vector<int16_t> result;
    int row = 0;
    int col = 0;
    int min = 10;
    int missing = 0;
    std::vector<int16_t> availables(DIM, 0);
    for (int i = 0; i < DIM; ++i) {
        int16_t value = 0;
        int i_subgrid = (i / DIM_SUBGRID) * 3;
        for (int j = 0; j < DIM; ++j) {
            int j_subgrid = j / DIM_SUBGRID;
            if (conf.grid[i][j] == 0) {
                missing++;
                std::vector<int16_t> availables_tmp(DIM, 0);

                for (int v = 0; v < DIM; ++v) {
                    availables_tmp[v] = conf.rows_values[i][v] && conf.cols_values[j][v] &&
                                        conf.grids_values[i_subgrid + j_subgrid][v];
                    if (availables_tmp[v]) {
                        value = v;
                    }
                }
                int sum = std::accumulate(availables_tmp.begin(), availables_tmp.end(), 0);

                switch (sum) {
                    case 0:
                        row = -1;
                        col = -1;
                        min = 2;
                        break;
                    case 1:
                        Sudoku::fix_value(value + 1, i, j, conf);
                        missing--;
                        break;
                    default:
                        if (sum < min) {
                            min = sum;
                            row = i;
                            col = j;
                            availables = availables_tmp;
                        }
                        break;
                }
            }

            if (min == 2 || row == -1) {// best case we can find
                break;
            }
        }
        if (min == 2 || row == -1) {
            break;
        } // best case we can find


    }
    if (missing == 0) {
        row = -2;
        col = -2;
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

int one_step1(configuration &conf) {
    int result = 0;
    std::vector<int16_t> min_choices = brute_fix1(conf);
    if (min_choices[1] == -1) {
        result = -1;
    } else {
        if (min_choices[1] == -2) {
            result = -2;
        } else {
            for (int choice = 2; choice < min_choices.size(); ++choice) {
                configuration new_conf(conf);
                if (Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], new_conf) == 1) {
                    result = one_step1(new_conf);
                    if (result == -2) {
                        conf = new_conf;
                        break;
                    }
                }
            }
        }
    }
    return result;
}


void seq_solve(Sudoku &sudoku) {
    //one_step_seq2(sudoku.starting_conf,sudoku.solution);
    one_step_seq3(sudoku.starting_conf,sudoku.solution);
    //sudoku.print(sudoku.solution);
    //configuration *copy = new configuration(sudoku.starting_conf);
    //one_step_rec(*copy);
    //copy = one_step(copy);
    //sudoku.solution = *copy;
    //std::cout << "THE END" << std::endl;
    //delete copy;
    //Sudoku::print(sudoku.solution);
}


#endif //SPM_SUDOKUSEQ_H
