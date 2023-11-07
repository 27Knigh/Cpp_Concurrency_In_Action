#include <thread>
#include <iostream>

using namespace std;

void do_something(int& i) {
    ++i;
}

struct func {
    int& i; //引用类型，可能会存在安全隐患

    func(int& i_) : i(i_) {}

    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_something(i);
        }
    }
};

void oops() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func); //线程通过引用去修改局部变量的内容，可能会存在问题
    my_thread.detach();//尤其是在分离式的启动线程，可能会存在oops函数已经结束，
                       //但是线程my_thread还尝试访问和修改局部变量some_local_state
}

int main() {
    oops();
}
