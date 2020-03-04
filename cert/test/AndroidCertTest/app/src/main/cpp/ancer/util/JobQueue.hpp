#include "ThreadPool.hpp"
#include <vector>

using namespace ancer;

class Work
{
 public:
    Work() = default;
    virtual ~Work() = default;
    virtual void DoWork() {};
};

class JobQueue
{
 public:
    JobQueue();
    ~JobQueue();
    void RunAllReadiedJobs();
    void AddJob(Work*);

 private:
    std::mutex _lock;
    ThreadPool* threadPool;
    std::vector<Work*> jobs0;
    std::vector<Work*> jobs1;

    std::vector<Work*>* executingJobs;
    std::vector<Work*>* queueingJobs;
};