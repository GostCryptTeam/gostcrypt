#include <stdio.h>
#include <pthread.h>
#include <QtGlobal>
#include <stdlib.h>

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


void encrypt(quint64 size, quint8 *data, quint8 *ks){
	quint64 i;	
	for(i=0; i < size / BLOCK_SIZE; i++)
		gost_encrypt (data, data, (gost_kds *) ks);
}

int main(int argc, char **argv){
	
	quint32 i;	
	quint64 size = 500 * MB;
	quint8 * dataspace = malloc(BLOCK_SIZE);
	quint8 * key = malloc(KEY_SIZE);
	for(i=0; i<KEY_SIZE; i++) // assigning key
		key[i] = (quint8)i;

	gost_kds * ks = calloc(sizeof(gost_kds), 1);

	gost_set_key(key, ks); // creating key for gost

	encrypt(size, dataspace, (quint8 *)ks);
	
	printf("done.\n");

	return 0;
}
