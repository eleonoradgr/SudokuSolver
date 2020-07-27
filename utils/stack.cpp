#include <iostream>
#include <mutex>
#include <condition_variable>
#include <stack>
#include <vector>
#include <string>


template <typename T>
class systack
{
private:
    std::mutex              d_mutex;
    std::condition_variable d_condition;
    std::atomic<bool>       dismiss;
    std::stack<T>           d_stack;
public:

    systack(std::string s) { std::cout << "Created " << s << " stack " << std::endl;  }
    systack() {
        dismiss = false;
    }

    ~systack() {
        while(!d_stack.empty()){
            T rc(std::move(this->d_stack.top()));
            this->d_stack.pop();
            delete rc;
        }
    }

    void push(T const& value) {
        {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            d_stack.push(value);
        }
        this->d_condition.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        this->d_condition.wait(lock, [=]{ return ((!this->d_stack.empty()) || this->dismiss); });
        T rc = nullptr;
        if (!this->dismiss){
            rc = std::move(this->d_stack.top());
            this->d_stack.pop();
        }
        return rc;
    }

    int getValues(){
        return d_stack.size();
    }

    void dismiss_stack(){
        this->dismiss = true;
        this->d_condition.notify_all();
    }
};

