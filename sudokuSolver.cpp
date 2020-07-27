//
// Created by eleonora on 27/05/20.
//
#include "SudokuSeq.h"
#include "SudokuPTStack.h"
#include "SudokuPT.h"
#include "SudokuFF.h"
#include "utils/utimer.hpp"


int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cout << "usage: " << argv[0] << std::endl;
        std::cout << "filename: name of the file that contains starting configuration of sudoku (mandatory)"
                  << std::endl;
        std::cout << "n : Parallelism degree (mandatory)" << std::endl;
        std::cout << "mod : a string between {all, seq, pt_dfs, pt_mix, ff } (default all)" << std::endl;
        std::cout << "repetition : Number of times the experiment must be repeated (default 1)" << std::endl;
        std::cout << "print : Boolean for printing the starting configuration and the solution (default false)"
                  << std::endl;
        return -1;
    }
    std::string filename(argv[1]);
    int n = std::atoi(argv[2]);
    std::string mod = (argc < 4) ? "all" : argv[3];
    int repetition = (argc < 5) ? 1 : std::atoi(argv[4]);
    bool print = (argc < 6 ) ? false : (strcmp(argv[5], "true") == 0);

    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        int i = 1;
        while (std::getline(file, line)) {
            long seq = 0;
            long par_ff = 0;
            long par_pt_dfs = 0;
            long par_pt_mix = 0;
            for (int j = 0; j < repetition; ++j) {
                if (mod == "all" || mod == "seq") {
                    Sudoku sudoku1;
                    sudoku1.init(line);
                    if (print) Sudoku::print(sudoku1.starting_conf);
                    {
                        utimer time("sequential solution", &seq);
                        seq_solve(sudoku1);
                    }
                    if (print) Sudoku::print(sudoku1.solution);
                }

                if (mod == "all" || mod == "pt_dfs"){
                    Sudoku sudoku2;
                    sudoku2.init(line);
                    if (print) Sudoku::print(sudoku2.starting_conf);
                    {
                        utimer time("pt parallel solution dfs", &par_pt_dfs);
                        parallel_pt_dfs(sudoku2, n);
                    }
                    if (print) Sudoku::print(sudoku2.solution);
                }


                if (mod == "all" || mod == "pt_mix"){
                    Sudoku sudoku3;
                    sudoku3.init(line);
                    if (print) Sudoku::print(sudoku3.starting_conf);
                    {
                        utimer time("pt parallel solution mix", &par_pt_mix);
                        parallel_pt_mix(sudoku3, n);
                    }
                    if (print) Sudoku::print(sudoku3.solution);
                }

                if (n >= 2 && (mod == "all" || mod == "ff")) {
                    Sudoku sudoku4;
                    sudoku4.init(line);
                    if (print) Sudoku::print(sudoku4.starting_conf);
                    {
                        utimer time("ff parallel solution", &par_ff);
                        parallel_ff_solve(sudoku4, n);
                    }
                    if (print) Sudoku::print(sudoku4.solution);
                }

            }
            par_ff = par_ff / repetition;
            par_pt_dfs = par_pt_dfs / repetition;
            par_pt_mix = par_pt_mix / repetition;
            seq = seq / repetition;
            std::cout << "type,nr_sudoku, parallelism degree, time" << std::endl;
            if (mod == "all" || mod == "seq"){
                std::cout << "seq," << std::to_string(i) << "," << std::to_string(n) << "," << seq << std::endl;
            }
            if (mod == "all" || mod == "pt_dfs"){
                std::cout << "par_pt_dfs," << std::to_string(i) << "," << std::to_string(n) << "," << par_pt_dfs
                          << std::endl;
            }
            if (mod == "all" || mod == "pt_mix"){
                std::cout << "par_pt_mix," << std::to_string(i) << "," << std::to_string(n) << ","
                          << par_pt_mix << std::endl;
            }
            if (n >= 2 && (mod == "all" || mod == "ff")){
                std::cout << "par_ff," << std::to_string(i) << "," << std::to_string(n) << "," << par_ff << std::endl;
            }

            i++;
        }
    } else {
        std::cout << "Initialization failed, not valid file." << std::endl;
    }

    return 0;
}