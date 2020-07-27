//
// Created by eleonora on 02/06/20.
//

#ifndef SPM_SUDOKUFF_H
#define SPM_SUDOKUFF_H

#include "Sudoku.hpp"
#include <ff/ff.hpp>
#include <ff/farm.hpp>

#define LIMIT_FF 320

using namespace ff;

struct Task{

    configuration *conf;
    bool solution = false;
};

struct Emitter: ff_monode_t<Task> {

    Emitter(configuration* starting_conf){
        solution = starting_conf;
    }

    ~Emitter(){
        delete solution;
    }

    int svc_init() {
        last = get_num_outchannels();
        ready.resize(last);
        for(size_t i=0; i<ready.size(); ++i) ready[i] = true;
        nready=ready.size();
        return 0;
    }

    Task* svc(Task *in) {
        ssize_t wid = get_channel_id();
        if (wid<0) { // tasks coming from input (the first stage), the id of the channel is -1

            int victim = selectReadyWorker(); // get a ready worker
            if (victim<0) data.push_back(reinterpret_cast<Task *const>(solution));  // no one ready
            else {
                Task * starting_task = new Task();
                starting_task->conf = solution;
                starting_task->solution = false;
                ff_send_out_to( starting_task, victim);
                ready[victim]=false;
                --nready;
            }
            return GO_ON;
        }
        assert(ready[wid] == false);

        if (in->solution){
            solution = in->conf;
            delete in;
            eos_received = true;
            ready[wid] = true;
            ++nready;
        }else{
            if (!in->solution && in->conf == nullptr){
                delete in;
                ready[wid] = true;
                ++nready;
            }else{
                data.push_back(in);
            }
            int victim = selectReadyWorker(); // get a ready worker
            while (victim >= 0){
                if (! eos_received && data.size()>0 && victim >= 0) {
                    if (data.size() >= LIMIT_FF)
                        data.back()->solution= true;
                    ff_send_out_to(data.back(), victim);
                    data.pop_back();
                    ready[victim]=false;
                    --nready;
                    victim = selectReadyWorker();
                }else{
                    victim = -1;
                }
            }

        }
        if (eos_received && nready == ready.size()){
            broadcast_task(EOS);
        }
        return GO_ON;
    }

    void svc_end() {
        // just for debugging
        for (auto *elem : data){
            delete elem->conf;
            delete elem;
        }
        data.clear();
        //assert(data.size()==0);
    }

    int selectReadyWorker() {
        for (unsigned i=last+1;i<ready.size();++i) {
            if (ready[i]) {
                last = i;
                return i;
            }
        }
        for (unsigned i=0;i<=last;++i) {
            if (ready[i]) {
                last = i;
                return i;
            }
        }
        return -1;
    }

    void eosnotify(ssize_t id) {
         // we have to receive all EOS from the previous stage
            // EOS is coming from the input channel

            eos_received=true;
            if (eos_received              &&
                nready == ready.size()    ) {
                broadcast_task(EOS);
            }
    }

    bool  eos_received = false;
    configuration* solution = nullptr;
    unsigned last, nready;
    std::vector<bool> ready;   // ready flags
    std::vector<Task*> data;  // storage
};


struct Worker: ff_monode_t<Task> {
    Task* svc(Task * task) {
        int found = 0;
        while (!found) {
            int new_fixed = 1;
            while (new_fixed > 0) {
                new_fixed = fix_valid_values(*task->conf);
            }

            if (new_fixed == 0) {
                std::vector<int16_t> min_choices = brute_fix(*task->conf);
                if (!task->solution){
                    for (int choice = 3; choice < min_choices.size(); ++choice) {
                        configuration *new_conf = new configuration(*task->conf);
                        Sudoku::fix_value(min_choices[choice], min_choices[0], min_choices[1], *new_conf);
                        Task * result_task = new Task();
                        result_task->conf = new_conf;
                        result_task->solution = false;
                        ff_send_out_to(result_task,0);
                    }

                    Sudoku::fix_value(min_choices[2], min_choices[0], min_choices[1], *task->conf);
                }else{
                    new_fixed = rec_sol_seq(*(task->conf));
                }
            }

            if (new_fixed == -1) {
                delete task->conf;
                delete task;
                Task * result_task = new Task();
                result_task->conf = nullptr;
                result_task->solution = false;
                ff_send_out_to(result_task,0);
                break;
            }
            if (new_fixed == -2) {
                found++;
                task->solution = true;
                ff_send_out_to(task,0);
            }
        }
        return GO_ON;
    }
};


void parallel_ff_solve (Sudoku &sudoku, int n){

    const size_t nworkers = n-1;
    configuration * starting_c = new configuration(sudoku.starting_conf);
    Emitter emitter(starting_c);

    std::vector<std::unique_ptr<ff_node> > W;
    for(size_t i=0;i<nworkers;++i) W.push_back(make_unique<Worker>());

    ff_Farm<Task> farm(std::move(W));
    farm.add_emitter(emitter);
    farm.remove_collector(); // needed because the collector is present by default in the ff_Farm
    farm.wrap_around();   // this call creates feedbacks from Workers to the Emitter
    //farm.set_scheduling_ondemand(); //no good in this case


    if (farm.run_and_wait_end()<0) {
        error("running farm");
    }
    //Sudoku::print(*(emitter.solution));
    //delete starting_c;
    sudoku.solution =*(emitter.solution);
}

#endif //SPM_SUDOKUFF_H
