#include "thread_pool.hpp"
using namespace std;

int main() {
    ThreadPool tp(5);
    tp.init();
    future<int> fu[10];
    for (int i=0;i<10;i++){
        fu[i] = tp.submit([]{return 5;});
    }
    for (int i=0;i<10;i++){
        cout<<fu[i].get()<<"\n";
    }
    tp.destruct_();
    return 0;
}