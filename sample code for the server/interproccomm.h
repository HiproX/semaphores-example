#ifndef INTERPROCCOMM_H
#define INTERPROCCOMM_H

#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>

// [Shared Memory] Errors
#define ERR_CREAT_SHM 100
#define ERR_ATTACH_SHM 101
#define ERR_DELETE_SHM 102
#define	ERR_CONNECT_SHM 103
#define ERR_DISCONNECT_SHM 104
// [Semaphore] Errors
#define ERR_DELETE_SEM 200

// Error message
void error_msg(const char *_msg, int _codeError)
{
	fprintf(stderr, "%s\n", _msg);
	exit(_codeError);
}

// [Shared Memory] For Servers
int SharedMemoryCreate(int _key, int _size)
{
	int shmid = 0;
	if ((shmid = shmget(_key, _size, IPC_CREAT | 0666)) < 0)
	{
		error_msg("Can't create shared memory segment!", ERR_CREAT_SHM);
	}
	return shmid;
}

void* SharedMemoryAttach(int _shmid)
{
	void *shm = NULL;
	if ((shm = shmat(_shmid, NULL, 0)) == (int*)-1)
	{
		error_msg("Can't attach shared memory segment!", ERR_ATTACH_SHM);
	}
	return shm;
}

void SharedMemoryDelete(int _shmid)
{
	if (shmctl(_shmid, IPC_RMID, 0) < 0)
	{
		error_msg("Can't delete shared memory segment", ERR_DELETE_SHM);
	}
}

// [Shared Memory] Clients
int SharedMemoryConnect(int _key, int _size)
{
	int shmid = 0;
	if ((shmid = shmget(_key, _size, 0666)) < 0)
	{
		error_msg("Can't connect to shared memory segment", ERR_CONNECT_SHM);
	}
	return shmid;
}

void SharedMemoryDisconnect(void *_shm)
{
	if (shmdt(_shm) < 0)
	{
		error_msg("Can't disconnect shated memory segment", ERR_DISCONNECT_SHM);
	}
}

// [Semaphore] Servers
int SemaphoreCreate(int _key, int _semnum)
{
	return semget(_key, _semnum, IPC_CREAT | 0666);
}

void SemaphoreDelete(int _semid)
{
	if (semctl(_semid, 0, IPC_RMID) < 0)
	{
		error_msg("Can't delete semaphore!", ERR_DELETE_SEM);
	}
}

// [Semaphore] Clients
int SemaphoreConnect(int _key)
{
	return semget(_key, 0, 0666);
}

#endif
