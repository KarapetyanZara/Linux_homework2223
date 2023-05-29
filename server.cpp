#include <iostream>
#include <climits>
#include <sys/mman.h>
#include <sys/stat.h>      
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <queue>
#include <vector>
#include <signal.h>

int add(int num_1, int num_2)
{
    return num_1 + num_2;
}
int sub(int num_1, int num_2)
{
    return num_1 - num_2;
}
int mul(int num_1, int num_2)
{
    return num_1 * num_2;
}
int division(int num_1, int num_2)
{
    if(num_2 == 0)
    {
      std::cerr << "Can't divide by 0" << "\n";
      return INT_MIN;
    }
  return num_1 / num_2;
}
 class Task
{
public:
      int id;
      int arg_1;
      int arg_2;
      int result;
};

  class ThreadPool 
{
    private:
            std::queue<Task*> task_queue;
            std::vector<pthread_t*> threads;
            int threads_count;
            pthread_mutex_t* lock;
            pthread_cond_t* has_function;

    public:
            ThreadPool(int threads_count)
            {
                if(pthread_mutex_init(lock, nullptr) < 0)
                {
                  std::cerr << "Mutex init failed";
                }
                if(pthread_cond_init(has_function, nullptr) < 0)
                {
                  std::cerr << "Cond init failed";
                }
                for(int i = 0; i < threads_count; ++i)
                {
                  pthread_create(this->threads[i], nullptr, this->execute, this);
                }
            }
            ~ThreadPool()
            {
                pthread_cond_destroy(has_function);
                pthread_mutex_destroy(lock);
                for(int i = 0; i < this->threads_count; ++i)
                {
                  pthread_kill(*threads[i], SIGKILL);
                }
            }
            void add_task(Task* task)
            {
                pthread_mutex_lock(lock);
                this->task_queue.push(task);
                pthread_mutex_unlock(lock);
                pthread_cond_signal(has_function);
            }


            //void stop(){} 


            static void* execute(void* arg)
            {
                ThreadPool* threadpool = (ThreadPool*) arg;
                while(true)
                  {
                    pthread_mutex_lock(threadpool->lock);
                    while(threadpool->task_queue.empty())
                      {
                        pthread_cond_wait(threadpool->has_function, threadpool->lock);
                      }
                    Func* task = threadpool->task_queue.front();
                    threadpool->task_queue.pop();
                    pthread_mutex_unlock(threadpool->lock);

                  }
            }
};
int main()
{
  ThreadPool threadpool(5);


  const char* fname = "/shmem";
  int shmem_fd = shm_open(fname, O_RDWR|O_CREAT, 0666);
  if(shmem_fd == -1)
  {
    std::cerr << "Failed to open file" << "\n";
    exit(errno);
  }
  int shmem_size = sizeof(Task);
  if(ftruncate(shmem_fd, shmem_size) == -1)
  {
    std::cerr << "ftruncate id failed" << "\n";
    exit(errno);
  }
   Task* task = (Task*)mmap(nullptr, shmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
   if(task == MAP_FAILED)
   {
     std::cerr << "Failed to map file" << "\n";
     exit(errno);
   }
  close(shmem_fd);

  const char* sem_name_1 = "/sem_1";
  const char* sem_name_2 = "/sem_2";
  sem_t* sem_1 = sem_open(sem_name_1, O_CREAT, 0666, 0);
  sem_t* sem_2 = sem_open(sem_name_2, O_CREAT, 0666, 0);
  if(sem_1 == SEM_FAILED)
  {
    std::cerr << "Failed to open semaphore" << "\n";
    exit(errno);
  }
  if(sem_2 == SEM_FAILED)
  {
    std::cerr << "Failed to open semaphore" << "\n";
    exit(errno);
  }
  while(true)
  {
      sem_wait(sem_1);
      if(task->id > 3 || task->id < 0)
      {
        std::cout << "Wrong input" << "\n";
        func->id = INT_MIN;
        sem_post(sem_2);
        //exit(EXIT_FAILURE);
      }

      if(task->id == 0)
      {
        task->result = add(task->arg_1, task->arg_2);
      }
      else if(task->id == 1)
      {
        task->result = sub(task->arg_1, task->arg_2);
      }
      else if(task->id == 2)
      {
        task->result = mul(task->arg_1, task->arg_2);
      }
      else if(task->id == 3)
      {
        task->result = division(task->arg_1, task->arg_2);
      }
      sem_post(sem_2);
      if(task->id == 3 && task->result == INT_MIN)
      {
        exit(EXIT_FAILURE);
      }
  } 
  sem_close(sem_1);
  sem_close(sem_2);
  munmap(task, shmem_size);
  sem_unlink(sem_name_1);
  sem_unlink(sem_name_2);
  shm_unlink(fname);
  return 0;
}