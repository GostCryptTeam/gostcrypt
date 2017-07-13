/***********************************************************************************/
/* Implementation of Gost Grasshopper Encryption Algorithm                         */
/* This program implements the test vectors set provided in the reference document */
/* provided by the Standartinform, Moscow, Russian Federation.                     */
/*                                                                                 */
/* Author: Eric Filiol                                                             */
/* This program is protected by the GPLv3                                          */
/***********************************************************************************/

/***********************************************************************************/
/* Data structures for the gost_grasshopper algorithm.                             */
/* Each block is an array of two 64-bit integers.                                  */
/* Key is an array of four 64-bit integers.                                        */
/* Index 0 contains the most significant 64-bit word while index 1 or 3 contains   */
/* the least significant 64-bit word.                                              */
/* All functions have the same name as in the reference document                   */
/***********************************************************************************/

#ifndef GOST_GRASSHOPPER_H
#define GOST_GRASSHOPPER_H

//Structure of gost subkeys. 10 subkeys of 2 parts
typedef struct grasshopper_kds
{
	unsigned long y000; //K[0][0]
	unsigned long y001; //K[0][0]
	unsigned long y010; //K[0][1]
	unsigned long y011; //K[0][0]
	unsigned long y100; //K[1][0]
	unsigned long y101; //K[0][0]
	unsigned long y110; //K[1][1]
	unsigned long y111; //K[0][0]
	unsigned long y200; //K[2][0]
	unsigned long y201; //K[0][0]
	unsigned long y210; //K[2][1]
	unsigned long y211; //K[0][0]
	unsigned long y300; //K[3][0]
	unsigned long y301; //K[0][0]
	unsigned long y310; //K[3][1]
	unsigned long y311; //K[0][0]
	unsigned long y400; //K[4][0]
	unsigned long y401; //K[0][0]
	unsigned long y410; //K[4][1]
	unsigned long y411; //K[0][0]
	unsigned long y500; //K[5][0]
	unsigned long y501; //K[0][0]
	unsigned long y510; //K[5][1]
	unsigned long y511; //K[0][0]
	unsigned long y600; //K[6][0]
	unsigned long y601; //K[0][0]
	unsigned long y610; //K[6][1]
	unsigned long y611; //K[0][0]
	unsigned long y700; //K[7][0]
	unsigned long y701; //K[0][0]
	unsigned long y710; //K[7][1]
	unsigned long y711; //K[0][0]
	unsigned long y800; //K[8][0]
	unsigned long y801; //K[0][0]
	unsigned long y810; //K[8][1]
	unsigned long y811; //K[0][0]
	unsigned long y900; //K[9][0]
	unsigned long y901; //K[0][0]
	unsigned long y910; //K[9][1]
	unsigned long y911; //K[0][0]

} grasshopper_kds ;


/* The core nonlinear bijective permutation (S-Box) */
/* The corresponding inverse permutation            */

/****************************************************/
/* Base and conversion functions                    */
/****************************************************/

/* S conversion function                            */
void S(unsigned long * block);

/* Inverse of S conversion function                */
void S_inv(unsigned long * block);
/* Linear transformation                           */

unsigned char ll(unsigned long *block);

/* R conversion function                            */
void R(unsigned long * block);

/* Inverse of R conversion function                 */
void R_inv(unsigned long * block);

/* L conversion function (16 iteration of R)        */
void L(unsigned long * block);

/* Inverse of L conversion function (16 iteration of R_inv) */
void L_inv(unsigned long * block);

/* F conversion function (used in key setup)        */
void F(unsigned long * k, unsigned long * block1, unsigned long * block0);

/****************************************************/
/****************************************************/
/* The three core functions                         */
/****************************************************/
/****************************************************/

/****************************************************/
/* Key setup procedure                              */
/****************************************************/
void grasshopper_set_key(unsigned long * master_key, grasshopper_kds *subkeys);

/****************************************************/
/* Encryption procedure                             */
/****************************************************/
void grasshopper_encrypt(grasshopper_kds* subkeys, unsigned long * plaintext, unsigned long * ciphertext);

/****************************************************/
/* Decryption procedure                             */
/****************************************************/
void grasshopper_decrypt(grasshopper_kds* subkeys, unsigned long * ciphertext, unsigned long * plaintext);


void grasshopper_xor_ks(grasshopper_kds *ks, grasshopper_kds *out_ks, unsigned long *in, unsigned int len);

void changeEndian(unsigned long *ret);

#endif //GOST_GRASSHOPPER_H
