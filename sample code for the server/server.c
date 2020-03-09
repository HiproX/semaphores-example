#include <stdio.h>
#include <unistd.h>
#include "interproccomm.h" // Межпроцессорное взаимодействие (shm.h, sem.h и ф-и SharedMemory...)

#define VAR_T double // тип данных
#define SHM_KEY 9999
#define SEM_KEY 8888
#define SIZE 1

int glVlfct;
int factorial(int);
double combin(int, int);
void Process(VAR_T*);

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

const int N = 6;
const int K = 10;

struct sembuf lockRes = {0, -1, 0}; // блокировка ресурка
struct sembuf relRes = {0, 1, 0}; // освобождение ресурса

int main(int argc, char const *argv[])
{
	fputs("=====[ Server's try creating ]=====\n", stdout);
	union semun arg;
	// Создание семафора для синхронизации работы с разделяемой памятью
	int semid = SemaphoreCreate(SEM_KEY, 1);
	// Устанавливается в семафоре #0 (Контроллер ресурса) значение "1"
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);
	fputs(" > Semaphore created.\n", stdout);
	// Создание разделяемой памяти
	int shmid = SharedMemoryCreate(SHM_KEY, SIZE * sizeof(VAR_T));
	VAR_T *shm = SharedMemoryAttach(shmid);
	fputs(" > Shared Memory created.\n", stdout);
	fputs("=====[ Server created ]=====\n\n", stdout);
	fputs("Enter any key for start...\n", stdout);
	fgetc(stdin);

	// Блокировка разделяемой памяти
	if ((semop(semid, &lockRes, 1)) == -1)
		error_msg("Lock failed!", 555);
	else
		fputs("> Semaphore lock\n", stdout);

	// Обработка данных
	Process(shm);

	// Освобождение разделяемой памяти
	if ((semop(semid, &relRes, 1)) == -1)
		error_msg("Unlock failed!", 666);
	else 
		fputs("> Semaphore unlock\n", stdout);
	
	fputs("\n=====[ Server try delete ]=====\n", stdout);
	fputs("Enter any key for delete...\n", stdout);
	fgetc(stdin);
	SharedMemoryDisconnect(shm);
	fputs(" > Shared Memory disconnected.\n", stdout);
	SharedMemoryDelete(shmid);
	fputs(" > Shared Memory deleted.\n", stdout);
	SemaphoreDelete(semid);
	fputs(" > Semaphore deleted.\n", stdout);
	fputs("=====[ Server deleted ]=====\n", stdout);
	return 0;
}

void Process(VAR_T *shm)
{
	sleep(8); // эмуляция работы с данными
	// Запись данных в разделяемую память
	*shm = combin(N, K);
	fprintf(stdout, "%lf\n", *shm);
}

int factorial(int count){
    glVlfct = 1;
    for (int i = 1; i <= count; i++)
        glVlfct = glVlfct * i;
    return glVlfct;
}

double combin(int n, int k)
{ 
	return ((double)factorial(n) / (factorial(k) * factorial(n - k)));
}
