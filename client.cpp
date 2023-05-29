#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <iostream>
#include <climits>
#include <sys/mman.h>
#include <sys/stat.h>      

class Task
{
public:
      int id;
      int arg_1;
      int arg_2;
      int result;
};

int main()
{
  const char* filename = "/shmem";
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
  std::cout << "Enter the function number where 0 is add, 1 is sub, 2 is mul and 3 id division: ";
  std::cin >> task->id;
  if(task->id == INT_MIN)
  {
    std::cout<<"Function id must be from 0 to 3";
    exit(1);
  }
  std::cout << "Enter 2 arguments: ";
  std::cin >> task->arg_1 >> task->arg_2;
  sem_post(sem_1);
  sem_wait(sem_2);
  if(task->id == 3 && task->result == INT_MIN)
  {
    std::cout<< "0 division";
    exit(1);
  }
  std::cout << "The result is: " << task->result << "\n";
  sem_close(sem_1);
  sem_close(sem_2);
  munmap(task, shmem_size);

  return 0;
}
