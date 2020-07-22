//
// Created by eleonora on 17/05/20.
//
#ifndef __x86_64__
#warning "The program is developed for x86-64 architecture only."
#endif
#if !defined(DCACHE1_LINESIZE) || !DCACHE1_LINESIZE
#ifdef DCACHE1_LINESIZE
#undef DCACHE1_LINESIZE
#endif
#define DCACHE1_LINESIZE 64
#endif
#ifndef SPM_NBQUEUE_H
#define SPM_NBQUEUE_H

#include <vector>
#include <optional>
#include <mutex>
#include <malloc.h>
#include <climits>
#include <assert.h>

#define Q_SIZE    (100 * 1024)

template<class T>
class nbQueue {
public:
    nbQueue(uint8_t _readers, uint8_t _writers) : readers(_readers), writers(_writers),
                                                  head_(0),
                                                  tail_(0),
                                                  last_head_(0),
                                                  last_tail_(0) {
        auto n = std::max(_readers, _writers);
        thr_p_.reserve(n);

        // Set per thread tail and head to ULONG_MAX.
        for (auto i = 0; i < n; ++i) {
            thr_p_.push_back(ThrPos());
        }

        ptr_array_ = std::vector<T *>(Q_SIZE);
    }

    ~nbQueue(){
        if (tail_ < head_){
            while ((tail_&Q_MASK) != (head_ & Q_MASK)){
                delete ptr_array_[tail_ & Q_MASK];
                tail_++;
            }
            assert((head_ & Q_MASK)== (tail_&Q_MASK));
        }
    }


    void
    push(T *ptr, uint8_t index) {
        ThrPos &tp = thr_p_[index];

        //tp.head = head_.load();
        //tp.head = head_.fetch_add(1,  std::memory_order_seq_cst);
        //if (tp.head == ULLONG_MAX) {
            tp.head = head_;
            tp.head = __sync_fetch_and_add(&head_, 1);
        //}
        /*
         * We do not know when a consumer uses the pop()'ed pointer,
         * se we can not overwrite it and have to wait the lowest tail.
         */
        int attempts = 100;
        while (__builtin_expect(tp.head >= last_tail_ + Q_SIZE && attempts >0 , 0)) {
            attempts--;
            auto min = tail_;

            // Update the last_tail_.
            for (size_t i = 0; i < readers; ++i) {
                auto tmp_t = thr_p_[i].tail;

                // Force compiler to use tmp_h exactly once.
                asm volatile("":: : "memory");

                if (tmp_t < min)
                    min = tmp_t;
            }
            last_tail_ = min;

            if (tp.head < last_tail_ + Q_SIZE)
                break;
            ::sched_yield();
        }

        if (__builtin_expect(attempts>0,1)){
            ptr_array_[tp.head & Q_MASK] = ptr;
            // Allow consumers eat the item.
            tp.head = ULLONG_MAX;
        }

    }


    T *
    pop(uint8_t index) {
        assert(index < std::max(readers, writers));
        ThrPos &tp = thr_p_[index];

        if (tp.tail == ULLONG_MAX){
            /*
        * Loads and stores are not reordered with locked instructions,
        * se we don't need a memory barrier here.
        */
            tp.tail = tail_;
            tp.tail = __sync_fetch_and_add(&tail_, 1);

        }

        /*
         * last_head_ guaraties that no any consumer eats the item
         * before producer reserved the position writes to it.
         */
        int attempts = 100;
        while (__builtin_expect(tp.tail >= last_head_ && attempts >0 , 0)) {
            attempts--;
            auto min = head_;

            // Update the last_head_.
            for (size_t i = 0; i < writers; ++i) {
                auto tmp_h = thr_p_[i].head;

                // Force compiler to use tmp_h exactly once.
                asm volatile("":: : "memory");

                if (tmp_h < min)
                    min = tmp_h;
            }
            last_head_ = min;

            if (tp.tail < last_head_){
                attempts++;
                break;
            }
            ::sched_yield();
        }

        T *ret = nullptr;
        if (__builtin_expect(attempts>0,1)){
            ret = ptr_array_[tp.tail & Q_MASK];
            // Allow producers rewrite the slot.
            tp.tail = ULLONG_MAX;
        }

        return ret;
    }


    int getValues(){
        unsigned long tmp_head = head_;
        unsigned long tmp_tail = tail_;
        return (tmp_head - tmp_tail);
    }

private:

    static const unsigned long Q_MASK = Q_SIZE - 1;

    struct ThrPos {
        unsigned long head, tail;

        ThrPos() {
            head = ULLONG_MAX;
            tail = ULLONG_MAX;
        }
    };


    const uint8_t readers;
    const uint8_t writers;
    // currently free position (next to insert)
    unsigned long head_ __attribute__((aligned(DCACHE1_LINESIZE)));
    // current tail, next to pop
    unsigned long tail_ __attribute__((aligned(DCACHE1_LINESIZE)));
    // last not-processed producer's pointer
    unsigned long last_head_ __attribute__((aligned(DCACHE1_LINESIZE)));
    // last not-processed consumer's pointer
    unsigned long last_tail_ __attribute__((aligned(DCACHE1_LINESIZE)));
    std::vector<ThrPos> thr_p_;
    std::vector<T *> ptr_array_;

};


#endif //SPM_NBQUEUE_H
