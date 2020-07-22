//
// Created by eleonora on 27/05/20.
//
#include "SudokuSeq.h"
#include "SudokuPTBlock.h"
#include "SudokuPT.h"
#include "SudokuFF.h"
#include "utils/utimer.hpp"

int filter(configuration &conf,int i){
    int new_fixed = 1;
    while (new_fixed > 0) {
        new_fixed = fix_valid_values(conf);
        if (new_fixed >= 0) {
            int to_fix_for_unicity = fix_unique_values(conf);
            new_fixed = (to_fix_for_unicity == -1) ? -1 : std::max(new_fixed, to_fix_for_unicity);
        }
    }
    if (new_fixed == -2) {
        //starting_conf = conf;
        std::cout << i << std::endl;
    }
    return new_fixed;
}

int main(int argc, char* argv[]){

    if (argc < 4) {
        std::cout << "usage: " << argv[0] << std::endl;
        std::cout << "filename: name of the file that contains starting configuration of sudoku (mandatory)" << std::endl;
        std::cout << "n : Parallelism degree (mandatory)" << std::endl;
        std::cout << "smart_memory : bool in limit in usage of memory (default false)" << std::endl;
        std::cout << "smart_seq : bool for adding costraints verification in sequential version (default false)" << std::endl;
        return -1;
    }
    std::string filename(argv[1]);
    int n = std::atoi(argv[2]);
    int repetition = 10;

    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        int i = 1;
        while (std::getline(file, line)) {
            long parallel_ff= 0;
            long parallel_pt= 0;
            long parallel_pt_block = 0;
            long seq = 0;
            for (int j = 0; j< repetition; ++j){
                Sudoku sudoku1;
                sudoku1.init(line);
                long seq_test = 0;
                Sudoku::print(sudoku1.starting_conf);
                {
                    utimer time("sequential solution", &seq);
                    seq_solve(sudoku1);
                }
                //std::cout << "seq,"<<std::to_string(i)<< "," <<std::to_string(seq_test)<<","<<line<<std::endl;
                //i++;
                //std::cout << "------------------"<<std::endl;
                //Sudoku::print(sudoku1.starting_conf);
                //std::cout << "------------------"<<std::endl;
                Sudoku::print(sudoku1.solution);

                Sudoku sudoku2;
                sudoku2.init(line);
                //Sudoku::print(sudoku2.starting_conf);
                {
                    utimer time("pt parallel solution", &parallel_pt);
                    parallel_pt_solve(sudoku2,n);
                }
                //std::cout << "------------------"<<std::endl;
                //Sudoku::print(sudoku2.starting_conf);
                //std::cout << "------------------"<<std::endl;
                //Sudoku::print(sudoku2.solution);

                Sudoku sudoku3;
                sudoku3.init(line);
                {
                    utimer time("pt blocking parallel solution", &parallel_pt_block);
                    parallel_pt_block_solve(sudoku3,n);
                }

                //if (n >= 2){
                //    Sudoku sudoku;
                //    sudoku.init(line);
                //Sudoku::print(sudoku.starting_conf);

                //    {
                //        utimer time("ff parallel solution", &parallel_ff);
                //        parallel_ff_solve(sudoku,n);
                //    }
                //}
                //std::cout << "------------------"<<std::endl;
                //Sudoku::print(sudoku.starting_conf);
                //std::cout << "------------------"<<std::endl;
                //Sudoku::print(sudoku.solution);
            }
            //parallel_ff= parallel_ff/repetition;
            parallel_pt= parallel_pt/repetition;
            parallel_pt_block = parallel_pt_block/repetition;
            seq = seq/repetition;
            std::cout <<"seq,"<< std::to_string(i)<< "," <<std::to_string(n) <<","<< seq << std::endl;
            std::cout <<"par_pt,"<< std::to_string(i)<< "," <<std::to_string(n) <<"," << parallel_pt << std::endl;
            std::cout <<"par_pt_block,"<< std::to_string(i)<< ","<<std::to_string(n) <<","<< parallel_pt_block << std::endl;
            //std::cout <<"par_ff,"<< std::to_string(i)<< ","<<std::to_string(n) <<","<< parallel_ff << std::endl;
            i++;
        }
    } else {
        std::cout << "Initialization failed, not valid file." << std::endl;
    }



    /*SudokuPT prova;
    prova.init(filename);
    std::cout << "------------------"<<std::endl;
    {
        utimer time("parallel solution");
        prova.solve();
    }

    SudokuSeq prova1;

    prova1.init(filename);
    std::cout << "------------------"<<std::endl;
    {
        utimer time("sequential solution");
        prova1.solve();
    }

    SudokuFF prova2;
    prova2.init(filename);
    std::cout << "------------------"<<std::endl;
    {
        utimer time("fastflow solution");
        prova2.solve();
    }
*/

    return 0;
}