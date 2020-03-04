#include "JobQueue.hpp"


using namespace ancer;
using ancer::ThreadPool;

JobQueue::JobQueue() {
    threadPool = new ThreadPool(ancer::ThreadAffinity::kAnyCore, true);
    executingJobs = &jobs0;
    queueingJobs = &jobs1;
}

JobQueue::~JobQueue() {
    delete threadPool;
}

void JobQueue::AddJob(Work* work) {
    std::lock_guard guard(_lock);
    queueingJobs->push_back(work);
}

void JobQueue::RunAllReadiedJobs() {
    std::lock_guard guard(_lock);
    std::vector<Work*>* temp = executingJobs;
    executingJobs = queueingJobs;
    queueingJobs = temp;

    for (int i = 0; i < executingJobs->size(); ++i)
    {

        threadPool->Enqueue([this, i](){
            (*executingJobs)[i]->DoWork();
        });
    }

    threadPool->Wait();

    for (int i = 0; i < executingJobs->size(); ++i)
    {
        delete (*executingJobs)[i];
    }

    executingJobs->clear();
}
