//
// Created by eleonora on 30/05/20.
//
#ifndef SPM_SUDOKUPT_H
#define SPM_SUDOKUPT_H


#include <thread>
#include <atomic>
#include "Sudoku.hpp"
#include "utils/nbQueue.h"
#include "utils/queue.cpp"

#define LIMIT_PT 320


int mixed_pt_block(configuration &starting_conf, syque<configuration *> &test_conf, std::atomic<int> &found, int i, int n) {
    while (!found.load()) {
        configuration *conf = test_conf.pop();
        if (conf != nullptr) {
            int sol = 0;
            while (!found.load()) {
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
                    starting_conf = *conf;
                    test_conf.dismiss_stack();
                    sol++;
                }

                if (new_fixed == 0) {
                    int dim = test_conf.getValues();
                    if (dim > LIMIT_PT) {
                        new_fixed = rec_sol_seq(*conf);
                        if (new_fixed == -1) {
                            delete conf;
                            break;
                        }
                        if (new_fixed == -2) {
                            found++;
                            starting_conf = *conf;
                            test_conf.dismiss_stack();
                            sol++;
                        }
                    } else {
                        std::vector<int16_t> min_choices = brute_fix(*conf);
                        for (int choice = 3; choice < min_choices.size(); ++choice) {
                            configuration *new_conf = new configuration(*conf);
                            Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                            test_conf.push(new_conf);
                            if (found.load()) {
                                break;
                            }
                        }
                        configuration *new_conf = new configuration(*conf);;
                        Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *new_conf);
                        delete conf;
                        conf = new_conf;
                    }
                }
            }
        }

    }
    return 1;
}

int mixed_pt_noblock(configuration &starting_conf, nbQueue<configuration> &test_conf, std::atomic<int> &found, int i) {
    while (!found.load()) {
        configuration *conf = test_conf.pop(i);
        if (conf != nullptr) {
            int sol = 0;
            while (!found.load()) {
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
                    starting_conf = *conf;
                    sol++;
                }

                if (new_fixed == 0) {
                    int prova = test_conf.getValues();
                    if (prova > LIMIT_PT) {
                        new_fixed = rec_sol_seq(*conf);
                        if (new_fixed == -1) {
                            delete conf;
                            break;
                        }
                        if (new_fixed == -2) {
                            found++;
                            starting_conf = *conf;
                            sol++;
                        }
                    } else {
                        std::vector<int16_t> min_choices = brute_fix(*conf);
                        for (int choice = 3; choice < min_choices.size(); ++choice) {
                            configuration *new_conf = new configuration(*conf);
                            Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                            test_conf.push(new_conf, i);
                            if (found.load()) {
                                break;
                            }
                        }
                        configuration *new_conf = new configuration(*conf);;
                        Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *new_conf);
                        delete conf;
                        conf = new_conf;
                    }
                }
            }
        }

    }
    return 1;
}

void parallel_pt_mix(Sudoku &sudoku, int n) {
    syque<configuration*> test_conf;
    configuration *copy = new configuration(sudoku.starting_conf);
    int limit_nw = n-1;
    if (n == 1)
        limit_nw++;
    test_conf.push(copy);
    std::vector<std::thread> tid;
    std::atomic<int> found(0);
    tid.reserve(limit_nw);
    for (int i = 0; i < limit_nw; ++i) {
        tid.push_back(std::thread(mixed_pt_block, std::ref(sudoku.solution), std::ref(test_conf), std::ref(found), i, n));
    }
    if (n > 1){
        mixed_pt_block( std::ref(sudoku.solution), std::ref(test_conf), std::ref(found), n-1, n);
    }
    for (auto &t : tid)
        t.join();

}


#endif //SPM_SUDOKUPT_H
