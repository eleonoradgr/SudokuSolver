//
// Created by eleonora on 27/06/20.
//

#ifndef SPM_SUDOKUPTSTACK_H
#define SPM_SUDOKUPTSTACK_H

#include <thread>
#include <atomic>
#include "Sudoku.hpp"
#include "utils/stack.cpp"

int DFS_pt_block(configuration &solution_conf, systack<configuration*> &test_conf, std::atomic<int> &found) {
    while (!found.load()) {
        configuration *conf = test_conf.pop();
        if (conf != nullptr) {
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
                    solution_conf = *conf;
                    test_conf.dismiss_stack();
                }

                if (new_fixed == 0) {
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
    return 1;
}

void parallel_pt_dfs(Sudoku &sudoku, int n) {
    systack<configuration*> test_conf;
    configuration *copy = new configuration(sudoku.starting_conf);
    test_conf.push(copy);
    std::vector<std::thread> tid;
    std::atomic<int> found(0);
    tid.reserve(n);
    for (int i = 0; i < n; ++i) {
        tid.push_back(std::thread(DFS_pt_block, std::ref(sudoku.solution), std::ref(test_conf), std::ref(found)));
    }
    for (auto &t : tid)
        t.join();
}

#endif //SPM_SUDOKUPTSTACK_H
