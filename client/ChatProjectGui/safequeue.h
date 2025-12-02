#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H


#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>

class SafeQueue
{
public:
    SafeQueue(size_t workers=4);

private:
    using task = std::function<void()>;
    std::vector<std::thread> ts;
    std::mutex mutex;
    std::queue<task> q;
    std::atomic<bool> stop;
};

#endif // SAFEQUEUE_H
