//
// Created by eleonora on 27/06/20.
//

#ifndef SPM_SUDOKUPTBLOCK_H
#define SPM_SUDOKUPTBLOCK_H

#define NUM_THREAD 32

#include <thread>
#include <atomic>
#include "SudokuPT.h"
#include "utils/queue.cpp"
#include "utils/stack.cpp"

int one_step_pt_block(configuration &starting_conf, systack<configuration*> &test_conf, std::atomic<int> &found, int i) {
    while (!found.load()) {
        configuration *conf = test_conf.pop();
        if (conf != nullptr) {
            int sol = 0;
            while (!found.load()) {
                int new_fixed = 1;
                while (new_fixed > 0) {
                    new_fixed = fix_valid_values_pt(*conf);
                    /*if(new_fixed >= 0){
                        int to_fix_for_unicity = fix_unique_values(*conf);
                        new_fixed = (to_fix_for_unicity == -1)? -1:std::max(new_fixed,to_fix_for_unicity);
                    }*/
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

                //TODO: sostituire il primo con elemento di minima scelta
                if (new_fixed == 0) {
                    int prova = test_conf.getValues();
                    if (prova > 10 * NUM_THREAD) {
                        new_fixed = one_step_seq(*conf);
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
                        std::vector<int16_t> min_choices = brute_fix_pt(*conf);
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
            //if (sol != 1) { delete conf; }
        }

    }
    return 1;
}

void parallel_pt_block_solve(Sudoku &sudoku, int n) {
    systack<configuration*> test_conf;
    configuration *copy = new configuration(sudoku.starting_conf);
    test_conf.push(copy);
    std::vector<std::thread> tid;
    std::atomic<int> found(0);
    tid.reserve(n);
    for (int i = 0; i < n; ++i) {
        tid.push_back(std::thread(one_step_pt_block, std::ref(sudoku.solution), std::ref(test_conf), std::ref(found), i));
    }
    sudoku.print(sudoku.solution);
    for (auto &t : tid)
        t.join();

}

#endif //SPM_SUDOKUPTBLOCK_H
