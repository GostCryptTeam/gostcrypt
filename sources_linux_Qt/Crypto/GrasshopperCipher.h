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

#if defined(__cplusplus)
extern "C"
{
#endif //defined __cplusplus

typedef unsigned char byte;
typedef short gst_word;
typedef int gst_dword;
typedef unsigned short gst_uword;
typedef unsigned int gst_udword;
typedef unsigned long long gst_ludword;

//Structure of gost subkeys. 10 subkeys of 2 parts
typedef struct grasshopper_kds
{
	gst_ludword x00; //K[0][0]
	gst_ludword x01; //K[0][1]
	gst_ludword x10; //K[1][0]
	gst_ludword x11; //K[1][1]
	gst_ludword x20; //K[2][0]
	gst_ludword x21; //K[2][1]
	gst_ludword x30; //K[3][0]
	gst_ludword x31; //K[3][1]
	gst_ludword x40; //K[4][0]
	gst_ludword x41; //K[4][1]
	gst_ludword x50; //K[5][0]
	gst_ludword x51; //K[5][1]
	gst_ludword x60; //K[6][0]
	gst_ludword x61; //K[6][1]
	gst_ludword x70; //K[7][0]
	gst_ludword x71; //K[7][1]
	gst_ludword x80; //K[8][0]
	gst_ludword x81; //K[8][1]
	gst_ludword x90; //K[9][0]
	gst_ludword x91; //K[9][1]

} grasshopper_kds ;


/* The core nonlinear bijective permutation (S-Box) */
/* The corresponding inverse permutation            */

/****************************************************/
/* Base and conversion functions                    */
/****************************************************/

/* S conversion function                            */
/* inline void S(unsigned long long * block);		*/

/* Inverse of S conversion function                */
/* inline void S_inv(unsigned long long * block);	*/
/* Linear transformation                           */

 /* inline unsigned char ll(unsigned long long block[2]); */

/* R conversion function                            */
/* inline void R(unsigned long long * block);	*/

/* Inverse of R conversion function                 */
/* inline void R_inv(unsigned long long * block); */

/* L conversion function (16 iteration of R)        */
/* inline void L(unsigned long long * block); */

/* Inverse of L conversion function (16 iteration of R_inv) */
/* inline void L_inv(unsigned long long * block); */

/* F conversion function (used in key setup)        */
/* inline void F(unsigned long long * k, unsigned long long * block1, unsigned long long * block0); */

/****************************************************/
/****************************************************/
/* The three core functions                         */
/****************************************************/
/****************************************************/

/****************************************************/
/* Key setup procedure                              */
/****************************************************/
void grasshopper_set_key(unsigned long long * master_key, grasshopper_kds *subkeys);

/****************************************************/
/* Encryption procedure                             */
/****************************************************/
void grasshopper_encrypt(grasshopper_kds* subkeys, unsigned long long * plaintext, unsigned long long * ciphertext);

/****************************************************/
/* Decryption procedure                             */
/****************************************************/
void grasshopper_decrypt(grasshopper_kds* subkeys, unsigned long long * ciphertext, unsigned long long * plaintext);


void grasshopper_xor_ks(grasshopper_kds *ks, grasshopper_kds *out_ks, unsigned long long *in, unsigned int len);

void changeEndian(unsigned long long *ret);
#if defined(__cplusplus)
}
#endif //defined __cplusplus

#endif //GOST_GRASSHOPPER_H
