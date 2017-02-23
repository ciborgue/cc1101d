#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <semaphore.h>

#include <cstring>
#include <stdexcept>

#include "SysSem.h"

static const int SEMAPHORE_WAIT = 3;

SysSem::SysSem(const char * use_name) {
	char tmp_path[PATH_MAX];
	char * ptr_path = tmp_path;
	int process_len = readlink("/proc/self/exe", tmp_path, sizeof tmp_path);
	if(process_len == -1) {
		throw std::runtime_error("can't read '/proc/self/exe'");
	}
	for (int i = 0; i < process_len; i++) {
		if (tmp_path[i] == '/') {
			ptr_path = tmp_path + i;
		}
	}
	strncpy(process_name, ptr_path, sizeof(process_name) - 1);
	process_name[sizeof(process_name) - 1] = '\0';

	if (use_name == NULL) {
		semaphore_name = process_name;
	} else {
		semaphore_name = (char *) use_name;
	}

	semaphore = sem_open(semaphore_name, O_CREAT, 0644, 1);
	if (semaphore == NULL) {
		throw std::runtime_error("Can't open semaphore");
	}
#ifdef CC1101_DEBUG_LOGGING_SYSSEM
	syslog(LOG_DEBUG, "ctor finished: name:%s process:%s",
			semaphore_name, process_name);
#endif
}
SysSem::~SysSem() {
	unlock();
#ifdef CC1101_DEBUG_LOGGING_SYSSEM
	syslog(LOG_DEBUG, "dtor finished: name:%s process:%s",
			semaphore_name, process_name);
#endif
}
void SysSem::lock() {
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		throw std::runtime_error("Can't get time");
	}
	ts.tv_sec += SEMAPHORE_WAIT; // 3 seconds max
	if (sem_timedwait(semaphore, &ts) != 0) {
		throw std::runtime_error("Can't get semaphore");
	}
#ifdef CC1101_DEBUG_LOGGING_SYSSEM
	int semaphore_value;
	sem_getvalue(semaphore, &semaphore_value);
	syslog(LOG_DEBUG, "lock:%s process:%s result:%d",
			semaphore_name, process_name, semaphore_value);
#endif
}
void SysSem::unlock() {
  sem_post(semaphore);
#ifdef CC1101_DEBUG_LOGGING_SYSSEM
  int semaphore_value;
  sem_getvalue(semaphore, &semaphore_value);
  syslog(LOG_DEBUG, "unlock:%s process:%s result:%d",
    semaphore_name, process_name, semaphore_value);
#endif
}
