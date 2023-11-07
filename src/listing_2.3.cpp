/*等待线程完成的另一种解决办法，使用RAII等待线程完成*/
#include <thread>
#include <iostream>

//使用RAII机制，实现该线程守护类
class thread_guard
{
    std::thread& t;
public:
    //只允许线程调用带参的构造函数
    explicit thread_guard(std::thread& t_):
        t(t_)
    {}
    ~thread_guard()
    {
        if(t.joinable())
        {
            t.join();
            std::cout << "线程已经join" << std::endl;
        }
    }
    //删除拷贝构造函数
    thread_guard(thread_guard const&)=delete;
    //删除赋值拷贝函数
    thread_guard& operator=(thread_guard const&)=delete;
};

void do_something(int& i)
{
    ++i;
}

struct func
{
    int& i;

    func(int& i_):i(i_){}

    void operator()()
    {
        for(unsigned j=0;j<1000000;++j)
        {
            do_something(i);
        }
    }
};

void do_something_in_current_thread(int& i)
{
    std::cout << "the value of some_local_state is : " << i  << std::endl;
}


void f()
{
    int some_local_state;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard g(t);//使用线程守护来管理线程t，这里由于是最后一个创建thread_guard对象，且是在栈空间上创建的，
                          // 由于栈的特性，那么当函数f快要退出的时候，首先会销毁thread_guard对象，那么就会调用
                          // thread_guard的析构函数，将线程tjoin
        
    do_something_in_current_thread(some_local_state);
}

int main()
{
    f();
}
