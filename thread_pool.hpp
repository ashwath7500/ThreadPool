#include <bits/stdc++.h>
using namespace std;

class ThreadPool{
public:
    ThreadPool(size_t num_threads) : pool_(vector<thread>(num_threads)), destroy_(false) {}
    void init() {
        for (size_t i = 0;i<pool_.size();i++) {
            pool_[i] = thread(&ThreadPool::worker_, this);
        }
    }
    template<typename func,typename...Args>
    auto submit(func&& f, Args&&... args) -> future<decltype(f(args...))> {
        function<decltype(f(args...))()> fun = bind(forward<func>(f),forward<Args>(args)...);
        auto task_ptr = make_shared<packaged_task<decltype(f(args...))()> >(fun);
        function<void()> task = [task_ptr]{
            (*task_ptr)();
        };
        {
            unique_lock lk(m_);
            tasks_.push(task);
        }
        cv_.notify_all();
        return (*task_ptr).get_future();
    }
    void destruct_() {
        destroy_ = true;
        cv_.notify_all();
        for (auto& it:pool_) it.join();
    }
private:
    vector<thread> pool_;
    queue<function<void()> > tasks_;
    void worker_() {
        function<void()> work;
        while(!destroy_) {
            bool working = false;
            {    
                unique_lock lk(m_);
                if (tasks_.empty()) {
                    cv_.wait(lk);
                }
                else {
                    work = tasks_.front();
                    tasks_.pop();
                    working = true;
                }
            }
            if (working){
                work();
                working = false;
            }
        }
    }
    mutex m_;
    condition_variable cv_;
    bool destroy_;
};