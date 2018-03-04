#include <stdio.h>
#include <pthread.h>
#include <QtGlobal>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "../gostcrypt2_src/Volume/Crypto/GostCipher.h"
#include "../gostcrypt2_src/Volume/Crypto/GrasshopperCipher.h"


#define B 8L
#define KB (1024L * B)
#define MB (1024L * KB)
#define GB (1024L * MB)

#define THREAD_NUMBER 8

struct parameters {
	quint64 size;
	quint8 *data; // data space
	void *ks; // subkey
	void (*encrypt_func)(quint64 size, quint8 *data, void *ks); // function to call
};

struct tester {
	void (*encrypt_func)(quint64 size, quint8 *data, void *ks);
	void (*set_key)(void *key, void *subkey);
	const char *name;
	quint32 keysize;
	quint32 blocksize;
	quint32 ks_size;
};

/*************** Custom functions to call ************/

/* GOST */
#define GOST_BLOCK_SIZE (8 * B)
#define GOST_KEY_SIZE (32 * B)

void gost_encryptfunc(quint64 size, quint8 *data, void *ks){
	quint64 i;	
	for(i=0; i < size / GOST_BLOCK_SIZE; i++) // (8 * B) is gost's block size
		gost_encrypt (data, data, (gost_kds*)ks);
}

void gost_set_keyfunc(void *key, void *subkey){
	gost_set_key((quint8*)key, (gost_kds*)subkey); // creating key for gost
}

/* GRASSHOPPER */

#define GRASSHOPPER_BLOCK_SIZE (8 * B)
#define GRASSHOPPER_KEY_SIZE (32 * B)

void grasshopper_encryptfunc(quint64 size, quint8 *data, void *ks){
	quint64 i;	
	for(i=0; i < size / GRASSHOPPER_BLOCK_SIZE; i++)
		grasshopper_encrypt ((grasshopper_kds*)ks, (unsigned long long *)data, (unsigned long long *)data);
}

void grasshopper_set_keyfunc(void *key, void *subkey){
	grasshopper_set_key((unsigned long long *)key, (grasshopper_kds*)subkey);
}

/******************************************************/

void *thread_encrypt(void *p){
	struct parameters *params = p;	
	params->encrypt_func(params->size, params->data, params->ks);
	return NULL;
}

void test_algorithm(struct tester *algorithm, quint64 size){
	quint32 i;
	struct timeval  tv1, tv2;

	/* Key Creation */
	quint8 *key = malloc(algorithm->keysize);
	for(i=0; i<algorithm->keysize; i++) // assigning key
		key[i] = (quint8)i;

	/* multiple thread vars */
	pthread_t thread[THREAD_NUMBER] = {0};
	struct parameters *params[THREAD_NUMBER] = {0};

	/* Preparing thread parameters */
	for(i = 0; i < THREAD_NUMBER; i++){
		params[i] = malloc(sizeof(struct parameters));
		params[i]->size = size / THREAD_NUMBER;
		params[i]->data = malloc(algorithm->blocksize);
		params[i]->ks = calloc(sizeof(algorithm->ks_size), 1);
		params[i]->encrypt_func = algorithm->encrypt_func;
		algorithm->set_key(key, params[i]->ks); // creating key for gost
	}

	/* Getting time before starting */
	gettimeofday(&tv1, NULL);
	
	/* creating all threads */
	for(i = 0; i < THREAD_NUMBER; i++){
		if(pthread_create(&thread[i], NULL, thread_encrypt, (void *)params[i])){
			printf("Thread creation error (%d).\n", i);
			return;
		}
	}
	
	/* waiting for all threads */
	for(i = 0; i < THREAD_NUMBER; i++){
		if (pthread_join(thread[i], NULL)) {
			printf("pthread_join error (%d).\n", i);
			return;
		}
	}

	/* Getting execution time */
	gettimeofday(&tv2, NULL);
	double timediff =	(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
				(double) (tv2.tv_sec - tv1.tv_sec);

	/* Final print */
	printf("%s : %d MB in %.2lf seconds\n", algorithm->name, size/MB, timediff);

	/* Freeing memory */
	for(i = 0; i < THREAD_NUMBER; i++){
		free(params[i]->data);
		free(params[i]->ks);
		free(params[i]);
	}
	free(key);
}

int main(int argc, char **argv){

	/* Algorithm initilization */
	struct tester gost;
	gost.encrypt_func = gost_encryptfunc;
	gost.set_key = gost_set_keyfunc;
	gost.name = "GOST";
	gost.blocksize = GOST_BLOCK_SIZE;
	gost.keysize = GOST_KEY_SIZE;
	gost.ks_size = sizeof(gost_kds);

	struct tester grasshopper;
	grasshopper.encrypt_func = grasshopper_encryptfunc;
	grasshopper.set_key = grasshopper_set_keyfunc;
	grasshopper.name = "Grasshopper";
	grasshopper.blocksize = GRASSHOPPER_BLOCK_SIZE;
	grasshopper.keysize = GRASSHOPPER_KEY_SIZE;
	grasshopper.ks_size = sizeof(grasshopper_kds);

	/* Size to encrypt */
	quint64 size = 1 * GB;
	
	/* Tests */
	test_algorithm(&grasshopper, size);
	test_algorithm(&gost, size);

	return 0;
}
