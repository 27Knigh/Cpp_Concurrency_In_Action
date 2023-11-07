/*joining_thread类的实现*/
#include <vector>
#include <thread>
#include <algorithm>
#include <functional>
#include <utility>

class joining_thread
{
    std::thread t;
public:
    joining_thread() noexcept=default;
    template<typename Callable, typename ... Args>
    explicit joining_thread(Callable&& func, Args&& ... args): 
    t(std::forward<Callable>(func), std::forward<Args>(args)...)
    {}
    explicit joining_thread(std::thread t_) noexcept: t(std::move(t_))
    {}
    joining_thread(joining_thread&& other) noexcept: t(std::move(other.t))
    {}
    joining_thread& operator=(joining_thread&& other) noexcept
    {
        if (joinable()) {
            join();
        }
        t = std::move(other.t);
        return *this;
    }
    joining_thread& operator=(std::thread other) noexcept
    {
        if (joinable())
            join();
        t = std::move(other);
        return *this;
    }
    ~joining_thread() noexcept
    {
        if (joinable())
            join();
    }

    void swap(joining_thread& other) noexcept
    {
        t.swap(other.t);
    }

    std::thread::id get_id() const noexcept
    {
        return t.get_id();
    }

    bool joinable() const noexcept
    {
        return t.joinable();
    }

    void join() 
    {
        t.join();
    }

    void detach()
    {
        t.detach();
    }

    std::thread& as_thread() noexcept
    {
        return t;
    }

    const std::thread& as_thread() const noexcept
    {
        return t;
    }
};


//量产线程，等待它们结束

void do_work(unsigned id)
{}

void f()
{
    std::vector<std::thread> threads;
    for(unsigned i=0;i<20;++i)
    {
        threads.push_back(std::thread(do_work,i));
    }
    std::for_each(threads.begin(),threads.end(),
        std::mem_fn(&std::thread::join));
    //std::mem_fn 是C++标准库中的函数模板，用于创建一个可调用对象（function object），用于调用类的成员函数。
    //这对于使用标准库算法和函数对象时非常有用，特别是当你需要在算法中调用成员函数时。
}

int main()
{
    f();
}
