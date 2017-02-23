#ifndef __SYSSEM_H__
#define __SYSSEM_H__
#include <limits.h>
#include <semaphore.h>

// #define CC1101_DEBUG_LOGGING_SYSSEM
class SysSem {
  private:
    char  process_name[NAME_MAX + 2]; // '/' + name + '\0'
    char  *semaphore_name = NULL;
    sem_t *semaphore;

  public:
    SysSem(const char *use_name = NULL);
    ~SysSem();

    void lock();
    void unlock();

    const char * semName() { return semaphore_name; }
    const char * imgName() { return process_name + 1; }
};
#endif
