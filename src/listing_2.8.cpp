/*运行时决定线程数量*/
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>

template<typename Iterator,typename T>
struct accumulate_block
{
    void operator()(Iterator first,Iterator last,T& result)
    {
        result=std::accumulate(first,last,result);//将区间[first,last)的值，累加到初值result
    }
};

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first,Iterator last,T init)
{
    unsigned long const length=std::distance(first,last); //求区间[first,last)的元素个数

    if(!length)
        return init;

    unsigned long const min_per_thread=25;
    unsigned long const max_threads=
        (length+min_per_thread-1)/min_per_thread;
    //用于获取系统支持的并行线程数（即CPU核心的数量）。
    //这个函数通常用于确定系统上可以有效利用的线程数量，以便进行并行处理和多线程编程。
    unsigned long const hardware_threads=std::thread::hardware_concurrency();

    unsigned long const num_threads=
        std::min(hardware_threads!=0?hardware_threads:2,max_threads);

    unsigned long const block_size=length/num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::thread>  threads(num_threads-1);

    Iterator block_start=first;
    for(unsigned long i=0;i<(num_threads-1);++i)
    {
        Iterator block_end=block_start;
        std::advance(block_end,block_size);//将迭代器block_end前进（正值）或后退（负值）block_size的距离
        threads[i]=std::thread(
            accumulate_block<Iterator,T>(),
            block_start,block_end,std::ref(results[i]));
        block_start=block_end;
    }
    accumulate_block<Iterator,T>()(block_start,last,results[num_threads-1]);
    
    std::for_each(threads.begin(),threads.end(),
        std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(),results.end(),init);
}

int main()
{
    std::vector<int> vi;
    for(int i=0;i<10;++i)
    {
        vi.push_back(10);
    }
    int sum=parallel_accumulate(vi.begin(),vi.end(),5);
    std::cout<<"sum="<<sum<<std::endl;
}
