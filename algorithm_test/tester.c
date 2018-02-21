#include <stdio.h>
#include <pthread.h>
#include <QtGlobal>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "../gostcrypt2_src/Volume/Crypto/GostCipher.h"


#define B 8L
#define KB (1024L * B)
#define MB (1024L * KB)
#define GB (1024L * MB)

/* GOST */
#define BLOCK_SIZE (8 * B)
#define KEY_SIZE (32 * B)
/**/

#define THREAD_NUMBER 8

struct parameters {
	quint64 size;
	quint8 *data;
	gost_kds *ks;
};

void encrypt(quint64 size, quint8 *data, gost_kds *ks){
	quint64 i;	
	for(i=0; i < size / BLOCK_SIZE; i++)
		gost_encrypt (data, data, ks);
}

void *thread_encrypt(void *p){
	struct parameters *params = p;	
	encrypt(params->size, params->data, params->ks);
	return NULL;
}

int main(int argc, char **argv){
	
	quint32 i;
	struct timeval  tv1, tv2;

	quint64 size = 4 * GB; // size to encrypt
	
	/* Key Creation */
	quint8 *key = malloc(KEY_SIZE);
	for(i=0; i<KEY_SIZE; i++) // assigning key
		key[i] = (quint8)i;

	/* multiple thread vars */
	pthread_t thread[THREAD_NUMBER];
	struct parameters *params[THREAD_NUMBER];

	/* Preparing thread parameters */
	for(i = 0; i < THREAD_NUMBER; i++){
		params[i] = malloc(sizeof(struct parameters));
		params[i]->size = size / THREAD_NUMBER;
		params[i]->data = malloc(BLOCK_SIZE);
		params[i]->ks = calloc(sizeof(gost_kds), 1);
		gost_set_key(key, params[i]->ks); // creating key for gost
	}

	/* Getting time before starting */
	gettimeofday(&tv1, NULL);
	
	/* creating all threads */
	for(i = 0; i < THREAD_NUMBER; i++){
		if(pthread_create(&thread[i], NULL, thread_encrypt, (void *)params[i])){
			printf("Thread creation error (%d).\n", i);
			return 1;
		}
	}
	
	/* waiting for all threads */
	for(i = 0; i < THREAD_NUMBER; i++){
		if (pthread_join(thread[i], NULL)) {
			printf("pthread_join error (%d).\n", i);
			return 2;
		}
	}

	/* Getting execution time */
	gettimeofday(&tv2, NULL);
	double timediff =	(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
				(double) (tv2.tv_sec - tv1.tv_sec);

	/* Final print */
	printf("GOST : %d MB in %.2lf seconds\n", size/MB, timediff);

	return 0;
}
