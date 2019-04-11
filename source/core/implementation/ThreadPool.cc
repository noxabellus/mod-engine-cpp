#include "../include/ThreadPool.hh"


namespace mod {
  s32_t ThreadPool::thread (ThreadPool* pool) {
    while (true) {
      mtx_lock_safe(&pool->queue_mtx);

      if (pool->jobs.count > 0) {
        size_t job_index = pool->jobs.count - 1;

        Job job = pool->jobs[job_index];
        
        pool->jobs.remove(job_index);

        mtx_unlock_safe(&pool->queue_mtx);
        
        job.callback(job.argument);
      } else {
        bool shutdown = pool->shutdown;

        mtx_unlock_safe(&pool->queue_mtx);

        if (shutdown) return 0;
      }
    }
  }


  ThreadPool::ThreadPool (size_t num_threads)
  : threads { }
  , jobs { }
  , shutdown(false)
  {
    mtx_init_safe(&queue_mtx, mtx_plain);

    for (size_t i = 0; i < num_threads; i ++) {
      thrd_t thrd;
      thrd_create(&thrd, (thrd_start_t) ThreadPool::thread, this);
      threads.append(thrd);
    }
  }

  void ThreadPool::destroy () {
    mtx_lock_safe(&queue_mtx);

    shutdown = true;
    
    mtx_unlock_safe(&queue_mtx);

    for (auto [ i, thrd ] : threads) {
      thrd_join_safe(thrd, NULL);
    }

    mtx_destroy(&queue_mtx);

    threads.destroy();
    jobs.destroy();
  }


  size_t ThreadPool::queue (Job::Callback callback, void* argument) {
    mtx_lock_safe(&queue_mtx);

    size_t index = jobs.count;

    jobs.append({ callback, argument });

    mtx_unlock_safe(&queue_mtx);

    return index;
  }


  void ThreadPool::await (size_t index) {
    while (true) {
      mtx_lock_safe(&queue_mtx);

      size_t remaining_jobs = jobs.count;

      mtx_unlock_safe(&queue_mtx);

      if (remaining_jobs <= index) break;
    }
  }

  void ThreadPool::await_all () {
    while (true) {
      mtx_lock_safe(&queue_mtx);

      size_t remaining_jobs = jobs.count;

      mtx_unlock_safe(&queue_mtx);

      if (remaining_jobs == 0) break;
    }
  }
}