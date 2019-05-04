#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "cstd.hh"
#include "Array.hh"

namespace mod {
  struct Job {
    using Callback = void (*) (void*);

    Callback callback;
    void* argument;
  };

  struct ThreadPool {
    Array<thrd_t> threads;
    Array<Job> jobs;
    mtx_t queue_mtx;
    bool shutdown;


    /* Create a new uninitialized ThreadPool */
    ThreadPool () { }

    /* Create a new initialized ThreadPool with the given number of threads */
    ENGINE_API ThreadPool (size_t num_threads);

    /* Clean up a ThreadPool and free its heap allocation, after waiting for all threads to complete their Jobs */
    ENGINE_API void destroy ();


    /* Queue a Job in a ThreadPool and return its index in the Job queue */
    ENGINE_API size_t queue (Job::Callback callback, void* argument);
    

    /* Wait for a single Job in a ThreadPool to complete, by blocking until the number of remaining Jobs is less than or equal to the given index */
    ENGINE_API void await (size_t index);

    /* Wait for all Jobs in a ThreadPool to complete, by blocking until the number of remaining Jobs is equal to zero */
    ENGINE_API void await_all ();


  private:
    /* The function used by each thread of a ThreadPool to iterate and execute Jobs */
    static ENGINE_API s32_t thread (ThreadPool* pool);
  };
}

#endif