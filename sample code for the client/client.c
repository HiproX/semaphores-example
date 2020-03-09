#include <stdio.h>
#include <unistd.h>
#include "interproccomm.h"

#define VAR_T double // тип данных
#define SHM_KEY 9999
#define SEM_KEY 8888
#define SIZE 1

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

struct sembuf lockRes = {0, -1, 0}; // блокировка ресурса
struct sembuf relRes = {0, 1, 0}; // освобождение ресурса

int main(int argc, char const *argv[])
{
	fputs("=====[ Client try connect ]=====\n", stdout);
	// Создание семафора для синхронизации работы с разделяемой памятью
	int semid = SemaphoreConnect(SEM_KEY);
	fputs(" > Semaphore connected.\n", stdout);
	int shmid = SharedMemoryConnect(SHM_KEY, SIZE * sizeof(VAR_T));
	VAR_T *shm = SharedMemoryAttach(shmid);
	fputs(" > Shared Memory connected.\n", stdout);
	fputs("=====[ Client connected ]=====\n\n", stdout);
	fputs("Enter any key for start...\n", stdout);
	fgetc(stdin);

	fputs(" Waiting for access to data...\n\n", stdout);

	// Блокировка разделяемой памяти
	if ((semop(semid, &lockRes, 1)) == -1)
		error_msg("Lock failed!", 555);
	else
		fputs("-----( Lock )-----\n", stdout);

	// Вывод значение из разделяемого сегмента памяти
	fprintf(stdout, " Data from Shared Memory: %lf\n", *shm);

	// Освобождение разделяемой памяти
	if ((semop(semid, &relRes, 1)) == -1)
		error_msg("Unlock failed!", 666);
	else 
		fputs("-----( Unlock )-----\n", stdout);
	
	fputs("\n=====[ Client try disconnect ]=====\n", stdout);
	fputs("Enter any key for disconnect...\n", stdout);
	fgetc(stdin);
	SharedMemoryDisconnect(shm);
	fputs(" > Shared Memory disconnected.\n", stdout);
	fputs("=====[ Client disconnected ]=====\n", stdout);
	return 0;
}
