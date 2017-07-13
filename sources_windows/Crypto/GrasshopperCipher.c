/**
 *
 *	\file GrasshopperCipher.c
 *	\brief Key setup, encryption and decryption procedure
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *
 *	Implementation of Gost Grasshopper Encryption Algorithm                        
 *	This program implements the test vectors set provided in the reference document
 *	provided by the Standartinform, Moscow, Russian Federation.                    
 *	Author: Eric Filiol                                                            
 *	This program is protected by the GPLv3
 *
 */


#include "GrasshopperCipher.h"
#include <stdio.h>

unsigned char pi[256] = {252, 238, 221,  17, 207, 110,  49,  22, 251, 196, 250, 218,  35, 197,   4,  77, 
	233, 119, 240, 219, 147,  46, 153, 186,  23,  54, 241, 187,  20, 205,  95, 193, 
	249,  24, 101,  90, 226,  92, 239,  33, 129,  28,  60,  66, 139,   1, 142,  79, 
	5, 132,   2, 174, 227, 106, 143, 160,   6,  11, 237, 152, 127, 212, 211,  31, 
	235,  52,  44,  81, 234, 200,  72, 171, 242,  42, 104, 162, 253,  58, 206, 204, 
	181, 112,  14,  86,   8,  12, 118,  18, 191, 114,  19,  71, 156, 183,  93, 135, 
	21, 161, 150,  41,  16, 123, 154, 199, 243, 145, 120, 111, 157, 158, 178, 177, 
	50, 117,  25,  61, 255,  53, 138, 126, 109,  84, 198, 128, 195, 189,  13,  87, 
	223, 245,  36, 169,  62, 168,  67, 201, 215, 121, 214, 246, 124,  34, 185,   3, 
	224,  15, 236, 222, 122, 148, 176, 188, 220, 232,  40,  80,  78,  51,  10,  74, 
	167, 151,  96, 115,  30,   0,  98,  68,  26, 184,  56, 130, 100, 159,  38,  65, 
	173,  69,  70, 146,  39,  94,  85,  47, 140, 163, 165, 125, 105, 213, 149,  59, 
	7,  88, 179,  64, 134, 172,  29, 247,  48,  55, 107, 228, 136, 217, 231, 137, 
	225,  27, 131,  73,  76,  63, 248, 254, 141,  83, 170, 144, 202, 216, 133,  97, 
	32, 113, 103, 164,  45,  43,   9,  91, 203, 155,  37, 208, 190, 229, 108,  82, 
	89, 166, 116, 210, 230, 244, 180, 192, 209, 102, 175, 194,  57,  75,  99, 182};

unsigned char pi_inv[256] = {165,  45,  50, 143,  14,  48,  56, 192,  84, 230, 158,  57,  85, 126,  82, 145,
	100,   3,  87,  90,  28,  96,   7,  24,  33, 114, 168, 209,  41, 198, 164,  63,
	224,  39, 141,  12, 130, 234, 174, 180, 154,  99,  73, 229,  66, 228,  21, 183,
	200,   6, 112, 157,  65, 117,  25, 201, 170, 252,  77, 191,  42, 115, 132, 213,
	195, 175,  43, 134, 167, 177, 178,  91,  70, 211, 159, 253, 212,  15, 156,  47,
	155,  67, 239, 217, 121, 182,  83, 127, 193, 240,  35, 231,  37,  94, 181,  30,
	162, 223, 166, 254, 172,  34, 249, 226,  74, 188,  53, 202, 238, 120,   5, 107,
	81, 225,  89, 163, 242, 113,  86,  17, 106, 137, 148, 101, 140, 187, 119,  60,
	123,  40, 171, 210,  49, 222, 196,  95, 204, 207, 118,  44, 184, 216,  46,  54,
	219, 105, 179,  20, 149, 190,  98, 161,  59,  22, 102, 233,  92, 108, 109, 173,
	55,  97,  75, 185, 227, 186, 241, 160, 133, 131, 218,  71, 197, 176,  51, 250,
	150, 111, 110, 194, 246,  80, 255,  93, 169, 142,  23,  27, 151, 125, 236,  88,
	247,  31, 251, 124,   9,  13, 122, 103,  69, 135, 220, 232,  79,  29,  78,   4,
	235, 248, 243,  62,  61, 189, 138, 136, 221, 205,  11,  19, 152,   2, 147, 128,
	144, 208,  36,  52, 203, 237, 244, 206, 153,  16,  68,  64, 146,  58,   1,  38,
	18,  26,  72, 104, 245, 129, 139, 199, 214,  32,  10,   8,   0,  76, 215, 116};


/* Precomputation of X.Y mod p(x) = x^8 + x^7 + x^6 + x + 1 where X and Y are elements in GF(2)[X]/p(x) */
unsigned char mult_mod_poly[8][256] = {

	{  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
		32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
		64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
		96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
		128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
		192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
		224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255},

	{  0,  16,  32,  48,  64,  80,  96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 195, 211, 227, 243, 131, 147, 163, 179,  67,  83,  99, 115,   3,  19,  35,  51,
		69,  85, 101, 117,   5,  21,  37,  53, 197, 213, 229, 245, 133, 149, 165, 181, 134, 150, 166, 182, 198, 214, 230, 246,   6,  22,  38,  54,  70,  86, 102, 118,
		138, 154, 170, 186, 202, 218, 234, 250,  10,  26,  42,  58,  74,  90, 106, 122,  73,  89, 105, 121,   9,  25,  41,  57, 201, 217, 233, 249, 137, 153, 169, 185,
		207, 223, 239, 255, 143, 159, 175, 191,  79,  95, 111, 127,  15,  31,  47,  63,  12,  28,  44,  60,  76,  92, 108, 124, 140, 156, 172, 188, 204, 220, 236, 252,
		215, 199, 247, 231, 151, 135, 183, 167,  87,  71, 119, 103,  23,   7,  55,  39,  20,   4,  52,  36,  84,  68, 116, 100, 148, 132, 180, 164, 212, 196, 244, 228,
		146, 130, 178, 162, 210, 194, 242, 226,  18,   2,  50,  34,  82,  66, 114,  98,  81,  65, 113,  97,  17,   1,  49,  33, 209, 193, 241, 225, 145, 129, 177, 161,
		93,  77, 125, 109,  29,  13,  61,  45, 221, 205, 253, 237, 157, 141, 189, 173, 158, 142, 190, 174, 222, 206, 254, 238,  30,  14,  62,  46,  94,  78, 126, 110,
		24,   8,  56,  40,  88,  72, 120, 104, 152, 136, 184, 168, 216, 200, 248, 232, 219, 203, 251, 235, 155, 139, 187, 171,  91,  75, 123, 107,  27,  11,  59,  43},

	{  0,  32,  64,  96, 128, 160, 192, 224, 195, 227, 131, 163,  67,  99,   3,  35,  69, 101,   5,  37, 197, 229, 133, 165, 134, 166, 198, 230,   6,  38,  70, 102,
		138, 170, 202, 234,  10,  42,  74, 106,  73, 105,   9,  41, 201, 233, 137, 169, 207, 239, 143, 175,  79, 111,  15,  47,  12,  44,  76, 108, 140, 172, 204, 236,
		215, 247, 151, 183,  87, 119,  23,  55,  20,  52,  84, 116, 148, 180, 212, 244, 146, 178, 210, 242,  18,  50,  82, 114,  81, 113,  17,  49, 209, 241, 145, 177,
		93, 125,  29,  61, 221, 253, 157, 189, 158, 190, 222, 254,  30,  62,  94, 126,  24,  56,  88, 120, 152, 184, 216, 248, 219, 251, 155, 187,  91, 123,  27,  59,
		109,  77,  45,  13, 237, 205, 173, 141, 174, 142, 238, 206,  46,  14, 110,  78,  40,   8, 104,  72, 168, 136, 232, 200, 235, 203, 171, 139, 107,  75,  43,  11,
		231, 199, 167, 135, 103,  71,  39,   7,  36,   4, 100,  68, 164, 132, 228, 196, 162, 130, 226, 194,  34,   2,  98,  66,  97,  65,  33,   1, 225, 193, 161, 129,
		186, 154, 250, 218,  58,  26, 122,  90, 121,  89,  57,  25, 249, 217, 185, 153, 255, 223, 191, 159, 127,  95,  63,  31,  60,  28, 124,  92, 188, 156, 252, 220,
		48,  16, 112,  80, 176, 144, 240, 208, 243, 211, 179, 147, 115,  83,  51,  19, 117,  85,  53,  21, 245, 213, 181, 149, 182, 150, 246, 214,  54,  22, 118,  86},

	{  0, 133, 201,  76,  81, 212, 152,  29, 162,  39, 107, 238, 243, 118,  58, 191, 135,   2,  78, 203, 214,  83,  31, 154,  37, 160, 236, 105, 116, 241, 189,  56,
		205,  72,   4, 129, 156,  25,  85, 208, 111, 234, 166,  35,  62, 187, 247, 114,  74, 207, 131,   6,  27, 158, 210,  87, 232, 109,  33, 164, 185,  60, 112, 245,
		89, 220, 144,  21,   8, 141, 193,  68, 251, 126,  50, 183, 170,  47,  99, 230, 222,  91,  23, 146, 143,  10,  70, 195, 124, 249, 181,  48,  45, 168, 228,  97,
		148,  17,  93, 216, 197,  64,  12, 137,  54, 179, 255, 122, 103, 226, 174,  43,  19, 150, 218,  95,  66, 199, 139,  14, 177,  52, 120, 253, 224, 101,  41, 172,
		178,  55, 123, 254, 227, 102,  42, 175,  16, 149, 217,  92,  65, 196, 136,  13,  53, 176, 252, 121, 100, 225, 173,  40, 151,  18,  94, 219, 198,  67,  15, 138,
		127, 250, 182,  51,  46, 171, 231,  98, 221,  88,  20, 145, 140,   9,  69, 192, 248, 125,  49, 180, 169,  44,  96, 229,  90, 223, 147,  22,  11, 142, 194,  71,
		235, 110,  34, 167, 186,  63, 115, 246,  73, 204, 128,   5,  24, 157, 209,  84, 108, 233, 165,  32,  61, 184, 244, 113, 206,  75,   7, 130, 159,  26,  86, 211,
		38, 163, 239, 106, 119, 242, 190,  59, 132,   1,  77, 200, 213,  80,  28, 153, 161,  36, 104, 237, 240, 117,  57, 188,   3, 134, 202,  79,  82, 215, 155,  30},

	{  0, 148, 235, 127,  21, 129, 254, 106,  42, 190, 193,  85,  63, 171, 212,  64,  84, 192, 191,  43,  65, 213, 170,  62, 126, 234, 149,   1, 107, 255, 128,  20,
		168,  60,  67, 215, 189,  41,  86, 194, 130,  22, 105, 253, 151,   3, 124, 232, 252, 104,  23, 131, 233, 125,   2, 150, 214,  66,  61, 169, 195,  87,  40, 188,
		147,   7, 120, 236, 134,  18, 109, 249, 185,  45,  82, 198, 172,  56,  71, 211, 199,  83,  44, 184, 210,  70,  57, 173, 237, 121,   6, 146, 248, 108,  19, 135,
		59, 175, 208,  68,  46, 186, 197,  81,  17, 133, 250, 110,   4, 144, 239, 123, 111, 251, 132,  16, 122, 238, 145,   5,  69, 209, 174,  58,  80, 196, 187,  47,
		229, 113,  14, 154, 240, 100,  27, 143, 207,  91,  36, 176, 218,  78,  49, 165, 177,  37,  90, 206, 164,  48,  79, 219, 155,  15, 112, 228, 142,  26, 101, 241,
		77, 217, 166,  50,  88, 204, 179,  39, 103, 243, 140,  24, 114, 230, 153,  13,  25, 141, 242, 102,  12, 152, 231, 115,  51, 167, 216,  76,  38, 178, 205,  89,
		118, 226, 157,   9,  99, 247, 136,  28,  92, 200, 183,  35,  73, 221, 162,  54,  34, 182, 201,  93,  55, 163, 220,  72,   8, 156, 227, 119,  29, 137, 246,  98,
		222,  74,  53, 161, 203,  95,  32, 180, 244,  96,  31, 139, 225, 117,  10, 158, 138,  30,  97, 245, 159,  11, 116, 224, 160,  52,  75, 223, 181,  33,  94, 202},

	{  0, 192,  67, 131, 134,  70, 197,   5, 207,  15, 140,  76,  73, 137,  10, 202,  93, 157,  30, 222, 219,  27, 152,  88, 146,  82, 209,  17,  20, 212,  87, 151,
		186, 122, 249,  57,  60, 252, 127, 191, 117, 181,  54, 246, 243,  51, 176, 112, 231,  39, 164, 100,  97, 161,  34, 226,  40, 232, 107, 171, 174, 110, 237,  45,
		183, 119, 244,  52,  49, 241, 114, 178, 120, 184,  59, 251, 254,  62, 189, 125, 234,  42, 169, 105, 108, 172,  47, 239,  37, 229, 102, 166, 163,  99, 224,  32,
		13, 205,  78, 142, 139,  75, 200,   8, 194,   2, 129,  65,  68, 132,   7, 199,  80, 144,  19, 211, 214,  22, 149,  85, 159,  95, 220,  28,  25, 217,  90, 154,
		173, 109, 238,  46,  43, 235, 104, 168,  98, 162,  33, 225, 228,  36, 167, 103, 240,  48, 179, 115, 118, 182,  53, 245,  63, 255, 124, 188, 185, 121, 250,  58,
		23, 215,  84, 148, 145,  81, 210,  18, 216,  24, 155,  91,  94, 158,  29, 221,  74, 138,   9, 201, 204,  12, 143,  79, 133,  69, 198,   6,   3, 195,  64, 128,
		26, 218,  89, 153, 156,  92, 223,  31, 213,  21, 150,  86,  83, 147,  16, 208,  71, 135,   4, 196, 193,   1, 130,  66, 136,  72, 203,  11,  14, 206,  77, 141,
		160,  96, 227,  35,  38, 230, 101, 165, 111, 175,  44, 236, 233,  41, 170, 106, 253,  61, 190, 126, 123, 187,  56, 248,  50, 242, 113, 177, 180, 116, 247,  55},

	{  0, 194,  71, 133, 142,  76, 201,  11, 223,  29, 152,  90,  81, 147,  22, 212, 125, 191,  58, 248, 243,  49, 180, 118, 162,  96, 229,  39,  44, 238, 107, 169,
		250,  56, 189, 127, 116, 182,  51, 241,  37, 231,  98, 160, 171, 105, 236,  46, 135,  69, 192,   2,   9, 203,  78, 140,  88, 154,  31, 221, 214,  20, 145,  83,
		55, 245, 112, 178, 185, 123, 254,  60, 232,  42, 175, 109, 102, 164,  33, 227,  74, 136,  13, 207, 196,   6, 131,  65, 149,  87, 210,  16,  27, 217,  92, 158,
		205,  15, 138,  72,  67, 129,   4, 198,  18, 208,  85, 151, 156,  94, 219,  25, 176, 114, 247,  53,  62, 252, 121, 187, 111, 173,  40, 234, 225,  35, 166, 100,
		110, 172,  41, 235, 224,  34, 167, 101, 177, 115, 246,  52,  63, 253, 120, 186,  19, 209,  84, 150, 157,  95, 218,  24, 204,  14, 139,  73,  66, 128,   5, 199,
		148,  86, 211,  17,  26, 216,  93, 159,  75, 137,  12, 206, 197,   7, 130,  64, 233,  43, 174, 108, 103, 165,  32, 226,  54, 244, 113, 179, 184, 122, 255,  61,
		89, 155,  30, 220, 215,  21, 144,  82, 134,  68, 193,   3,   8, 202,  79, 141,  36, 230,  99, 161, 170, 104, 237,  47, 251,  57, 188, 126, 117, 183,  50, 240,
		163,  97, 228,  38,  45, 239, 106, 168, 124, 190,  59, 249, 242,  48, 181, 119, 222,  28, 153,  91,  80, 146,  23, 213,   1, 195,  70, 132, 143,  77, 200,  10},

	{  0, 251,  53, 206, 106, 145,  95, 164, 212,  47, 225,  26, 190,  69, 139, 112, 107, 144,  94, 165,   1, 250,  52, 207, 191,  68, 138, 113, 213,  46, 224,  27,
		214,  45, 227,  24, 188,  71, 137, 114,   2, 249,  55, 204, 104, 147,  93, 166, 189,  70, 136, 115, 215,  44, 226,  25, 105, 146,  92, 167,   3, 248,  54, 205,
		111, 148,  90, 161,   5, 254,  48, 203, 187,  64, 142, 117, 209,  42, 228,  31,   4, 255,  49, 202, 110, 149,  91, 160, 208,  43, 229,  30, 186,  65, 143, 116,
		185,  66, 140, 119, 211,  40, 230,  29, 109, 150,  88, 163,   7, 252,  50, 201, 210,  41, 231,  28, 184,  67, 141, 118,   6, 253,  51, 200, 108, 151,  89, 162,
		222,  37, 235,  16, 180,  79, 129, 122,  10, 241,  63, 196,  96, 155,  85, 174, 181,  78, 128, 123, 223,  36, 234,  17,  97, 154,  84, 175,  11, 240,  62, 197,
		8, 243,  61, 198,  98, 153,  87, 172, 220,  39, 233,  18, 182,  77, 131, 120,  99, 152,  86, 173,   9, 242,  60, 199, 183,  76, 130, 121, 221,  38, 232,  19,
		177,  74, 132, 127, 219,  32, 238,  21, 101, 158,  80, 171,  15, 244,  58, 193, 218,  33, 239,  20, 176,  75, 133, 126,  14, 245,  59, 192, 100, 159,  81, 170,
		103, 156,  82, 169,  13, 246,  56, 195, 179,  72, 134, 125, 217,  34, 236,  23,  12, 247,  57, 194, 102, 157,  83, 168, 216,  35, 237,  22, 178,  73, 135, 124}};

/**
 *
 *	\fn void S(unsigned long * block)
 *	\brief Apply S conversion
 *	\param[in] unsigned long * block
 *	\return void
 *
 */
void S(unsigned long * block)
{
	unsigned long valtmp;

	valtmp  = ((unsigned long)(pi[(block[0] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi[(block[0] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi[(block[0] >>  8) & 0xFF]) <<  8);
	valtmp |= ((unsigned long)(pi[(block[0] >>  0) & 0xFF]) <<  0);
	block[0] = valtmp;

	valtmp  = ((unsigned long)(pi[(block[1] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi[(block[1] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi[(block[1] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi[ block[1] & 0xFF]);

	block[1] = valtmp;

	valtmp  = ((unsigned long)(pi[(block[2] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi[(block[2] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi[(block[2] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi[ block[2] & 0xFF]);

	block[2] = valtmp;

	valtmp  = ((unsigned long)(pi[(block[3] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi[(block[3] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi[(block[3] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi[ block[3] & 0xFF]);

	block[3] = valtmp;
}


/**
 *
 *	\fn void S_inv(unsigned long * block)
 *	\brief Inverse of S conversion
 *	\param[in] unsigned long * block
 *	\return void
 *
 */
void S_inv(unsigned long * block)
{
	unsigned long valtmp;

	valtmp  = ((unsigned long)(pi_inv[(block[0] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi_inv[(block[0] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi_inv[(block[0] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi_inv[ block[0] & 0xFF]);

	block[0] = valtmp;

	valtmp  = ((unsigned long)(pi_inv[(block[1] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi_inv[(block[1] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi_inv[(block[1] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi_inv[block[1] & 0xFF]);

	block[1] = valtmp;

	valtmp  = ((unsigned long)(pi_inv[(block[2] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi_inv[(block[2] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi_inv[(block[2] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi_inv[block[2] & 0xFF]);
	
	block[2] = valtmp;

	valtmp  = ((unsigned long)(pi_inv[(block[3] >> 24) & 0xFF]) << 24);
	valtmp |= ((unsigned long)(pi_inv[(block[3] >> 16) & 0xFF]) << 16);
	valtmp |= ((unsigned long)(pi_inv[(block[3] >>  8) & 0xFF]) <<  8);
	valtmp |=  (unsigned long)(pi_inv[ block[3] & 0xFF]);

	block[3] = valtmp;
}

/**
 *
 *	\fn unsigned char ll(unsigned long *block)
 *	\brief Get conversion value
 *	\param[in] unsigned long * block
 *	\return unsigned char val
 *
 */
unsigned char ll(unsigned long *block)
{
	unsigned char val;

	val  =  mult_mod_poly[4][(block[0] >> 24) & 0xFF];
	val ^=  mult_mod_poly[2][(block[0] >> 16) & 0xFF];
	val ^=  mult_mod_poly[3][(block[0] >>  8) & 0xFF];
	val ^=  mult_mod_poly[1][(block[0] >>  0) & 0xFF];

	val ^=  mult_mod_poly[6][(block[1] >> 24) & 0xFF];
	val ^=  mult_mod_poly[5][(block[1] >> 16) & 0xFF];
	val ^=  mult_mod_poly[0][(block[1] >>  8) & 0xFF];
	val ^=  mult_mod_poly[7][(block[1] >>  0) & 0xFF];

	val ^=  mult_mod_poly[0][(block[2] >> 24) & 0xFF];
	val ^=  mult_mod_poly[5][(block[2] >> 16) & 0xFF];
	val ^=  mult_mod_poly[6][(block[2] >>  8) & 0xFF];
	val ^=  mult_mod_poly[1][(block[2] >>  0) & 0xFF];

	val ^=  mult_mod_poly[3][(block[3] >> 24) & 0xFF];
	val ^=  mult_mod_poly[2][(block[3] >> 16) & 0xFF];
	val ^=  mult_mod_poly[4][(block[3] >>  8) & 0xFF];
	val ^=  mult_mod_poly[0][(block[3] >>  0) & 0xFF];

	return(val);
}

/**
 *
 *	\fn void R(unsigned long * block)
 *	\brief Apply R conversion
 *	\param[in] unsigned long * block
 *	\return void
 *
 */
void R(unsigned long * block)
{
	unsigned long valtmp[4];
	unsigned char val;

	valtmp[0] = block[0];
	valtmp[1] = block[1];
	valtmp[2] = block[2];
	valtmp[3] = block[3];
	val = ll(valtmp);

	//block[1] >>= 8;
	//block[1]  |= (((block[0]) & 0xFF) << 56);
	block[3] >>= 8;
	block[3] |= (block[2] & 0xFF) << 24;
	block[2] >>= 8;
	block[2] |= (block[1] & 0xFF) << 24;
	

//	block[0] >>= 8;
//	block[0]  |= ((unsigned long)(val) << 56);
	block[1] >>= 8;
	block[1] |= (block[0] & 0xFF) << 24;
	block[0] >>= 8;
	block[0] |= ((unsigned long int)(val & 0xFF) << 24);

}


/**
 *
 *	\fn void R_inv(unsigned long * block)
 *	\brief Inverse of R conversion
 *	\param[in] unsigned long * block
 *	\return void
 *
 */
void R_inv(unsigned long * block)
{
	unsigned long valtmp[4];
	unsigned char val;

	//valtmp[0]  = (block[0] << 8);
	//valtmp[0] |= ((block[1] >> 56) & 0xFF);
	valtmp[0] = block[0] << 8;
	valtmp[0] |= (block[1] >> 24) & 0xFF;
	valtmp[1] = block[1] << 8;
	valtmp[1] |= (block[2] >> 24) & 0xFF;

	//valtmp[1]  = (block[1] << 8);
	//valtmp[1] |= ((block[0] >> 56) & 0xFF);

	valtmp[2] = block[2] << 8;
	valtmp[2] |= (block[3] >> 24) & 0xFF;
	valtmp[3] = block[3] << 8;
	valtmp[3] |= (block[0] >> 24) & 0xFF;

	val = ll(valtmp);

	block[0]  = valtmp[0];
	block[1] = valtmp[1];
	block[2]  = (valtmp[2] & 0xFFFFFFFFL);
	block[3] = valtmp[3] & 0xFFFFFF00L;
	block[3] |= val;
}

/**
 *
 *	\fn void L(unsigned long * block)
 *	\brief L conversion (16 iteration of R)
 *	\param[in] unsigned long * block
 *	\return void
 *
 */
void L(unsigned long * block)
{
	R(block); R(block); R(block); R(block);
	R(block); R(block); R(block); R(block);
	R(block); R(block); R(block); R(block);
	R(block); R(block); R(block); R(block);
}

/**
 *
 *	\fn void L_inv(unsigned long * block)
 *	\brief Inverse of L conversion (16 iteration of R_inv)
 *	\param[in] unsigned long * block
 *	\return void
 *
 */
void L_inv(unsigned long * block)
{
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
}

/**
 *
 *	\fn void F(unsigned long * k, unsigned long * block1, unsigned long * block0)
 *	\brief F conversion (used in key setup)
 *	\param[in] unsigned long * block0
 *	\param[in] unsigned long * block1
 *	\param[in] unsigned long * k
 *	\return void
 *
 */
void F(unsigned long * k, unsigned long * block1, unsigned long * block0)
{
	unsigned long tmpblock[4];

	/* Bitwise xor of k with block2 */
	tmpblock[0] = (block1[0] ^ k[0]);
	tmpblock[1] = (block1[1] ^ k[1]);
	tmpblock[2] = (block1[2] ^ k[2]);
	tmpblock[3] = (block1[3] ^ k[3]);

	/* Apply S conversion function  */
	S(tmpblock);

	/* Apply L conversion function  */
	L(tmpblock);

	/* Add a0 to the the result     */
	tmpblock[0] ^= block0[0];
	tmpblock[1] ^= block0[1];
	tmpblock[2] ^= block0[2];
	tmpblock[3] ^= block0[3];

	/* Update the results           */
	block0[0] = block1[0];
	block0[1] = block1[1];
	block0[2] = block1[2];
	block0[3] = block1[3];

	block1[0] = tmpblock[0];
	block1[1] = tmpblock[1];
	block1[2] = tmpblock[2];
	block1[3] = tmpblock[3];
}

/*
 *
 * The three core functions
 *
 */

/**
 *
 *	\fn void grasshopper_set_key(unsigned long * master_key, grasshopper_kds *subkeys)
 *	\brief Key setup procedure
 *	\param[in] unsigned long * subkeys
 *	\param[in] unsigned long * master_key
 *	\return void
 *
 */
void grasshopper_set_key(unsigned long * master_key, grasshopper_kds *subkeys)
{

	unsigned long c_values[32][4], blocktmp[4], blocktmp2[4];
	unsigned char i;

	/* Compute the C_i values       */
	for(i = 0;i < 32;i++)
	{
		blocktmp[0] = 0L;
		blocktmp[1] = 0L;
		blocktmp[2] = 0L;
		blocktmp[3] = (unsigned long)(i + 1);
		L(blocktmp);
		c_values[i][0] = blocktmp[0]; c_values[i][1] = blocktmp[1]; c_values[i][2] = blocktmp[2]; c_values[i][3] = blocktmp[3];
	}


	/* Compute subkeys K_1 and K_2  */
	subkeys->y000 = master_key[0]; subkeys->y001 = master_key[1]; subkeys->y010 = master_key[2]; subkeys->y011 = master_key[3];
	subkeys->y100 = master_key[4]; subkeys->y101 = master_key[5]; subkeys->y110 = master_key[6]; subkeys->y111 = master_key[7];

	changeEndian(&subkeys->y000); changeEndian(&subkeys->y010); changeEndian(&subkeys->y001); changeEndian(&subkeys->y011);
	changeEndian(&subkeys->y100); changeEndian(&subkeys->y101); changeEndian(&subkeys->y110); changeEndian(&subkeys->y111);

	/* Compute subkeys K_3 and K_4  */
	blocktmp[0] = subkeys->y000; blocktmp[1] = subkeys->y001; blocktmp[2] = subkeys->y010; blocktmp[3] = subkeys->y011;
	blocktmp2[0] = subkeys->y100; blocktmp2[1] = subkeys->y101; blocktmp2[2] = subkeys->y110; blocktmp2[3] = subkeys->y111;
	F(c_values[0], blocktmp, blocktmp2); F(c_values[1], blocktmp, blocktmp2);
	F(c_values[2], blocktmp, blocktmp2); F(c_values[3], blocktmp, blocktmp2);
	F(c_values[4], blocktmp, blocktmp2); F(c_values[5], blocktmp, blocktmp2);
	F(c_values[6], blocktmp, blocktmp2); F(c_values[7], blocktmp, blocktmp2);

	subkeys->y200 = blocktmp[0]; subkeys->y201 = blocktmp[1]; subkeys->y210 = blocktmp[2]; subkeys->y211 = blocktmp[3];
	subkeys->y300 = blocktmp2[0]; subkeys->y301 = blocktmp2[1]; subkeys->y310 = blocktmp2[2]; subkeys->y311 = blocktmp2[3];

	/* Compute subkeys K_5 and K_6  */
	F(c_values[ 8], blocktmp, blocktmp2); F(c_values[ 9], blocktmp, blocktmp2);
	F(c_values[10], blocktmp, blocktmp2); F(c_values[11], blocktmp, blocktmp2);
	F(c_values[12], blocktmp, blocktmp2); F(c_values[13], blocktmp, blocktmp2);
	F(c_values[14], blocktmp, blocktmp2); F(c_values[15], blocktmp, blocktmp2);

	subkeys->y400 = blocktmp[0]; subkeys->y401 = blocktmp[1]; subkeys->y410 = blocktmp[2]; subkeys->y411 = blocktmp[3];
	subkeys->y500 = blocktmp2[0]; subkeys->y501 = blocktmp2[1]; subkeys->y510 = blocktmp2[2]; subkeys->y511 = blocktmp2[3];

	/* Compute subkeys K_7 and K_8  */
	F(c_values[16], blocktmp, blocktmp2); F(c_values[17], blocktmp, blocktmp2);
	F(c_values[18], blocktmp, blocktmp2); F(c_values[19], blocktmp, blocktmp2);
	F(c_values[20], blocktmp, blocktmp2); F(c_values[21], blocktmp, blocktmp2);
	F(c_values[22], blocktmp, blocktmp2); F(c_values[23], blocktmp, blocktmp2);

	subkeys->y600 = blocktmp[0]; subkeys->y601 = blocktmp[1]; subkeys->y610 = blocktmp[2]; subkeys->y611 = blocktmp[3];
	subkeys->y700 = blocktmp2[0]; subkeys->y701 = blocktmp2[1]; subkeys->y710 = blocktmp2[2]; subkeys->y711 = blocktmp2[3];

	/* Compute subkeys K_9 and K_{10} */
	F(c_values[24], blocktmp, blocktmp2); F(c_values[25], blocktmp, blocktmp2);
	F(c_values[26], blocktmp, blocktmp2); F(c_values[27], blocktmp, blocktmp2);
	F(c_values[28], blocktmp, blocktmp2); F(c_values[29], blocktmp, blocktmp2);
	F(c_values[30], blocktmp, blocktmp2); F(c_values[31], blocktmp, blocktmp2);

	subkeys->y800 = blocktmp[0]; subkeys->y801 = blocktmp[1]; subkeys->y810 = blocktmp[2]; subkeys->y811 = blocktmp[3];
	subkeys->y900 = blocktmp2[0]; subkeys->y901 = blocktmp2[1]; subkeys->y910 = blocktmp2[2]; subkeys->y911 = blocktmp2[3];

}


/**
 *
 *	\fn void grasshopper_encrypt(grasshopper_kds *subkeys, unsigned long * plaintext, unsigned long * ciphertext)
 *	\brief Encryption procedure
 *	\param[in] unsigned long * ciphertext
 *	\param[in] unsigned long * plaintext
 *	\param[in] grasshopper_kds * subkeys
 *	\return void
 *
 */
void grasshopper_encrypt(grasshopper_kds *subkeys, unsigned long * plaintext, unsigned long * ciphertext)
{
	/* Round 1 with subkey 1 */

	changeEndian(&plaintext[0]); changeEndian(&plaintext[1]); changeEndian(&plaintext[2]); changeEndian(&plaintext[3]);

	ciphertext[0] = plaintext[0] ^ subkeys->y000; ciphertext[1] = plaintext[1] ^ subkeys->y001; ciphertext[2] = plaintext[2] ^ subkeys->y010; ciphertext[3] = plaintext[3] ^ subkeys->y011;
	S(ciphertext); L(ciphertext);

	/* Round 2 with subkey 2 */
	ciphertext[0] ^= subkeys->y100; ciphertext[1] ^= subkeys->y101; ciphertext[2] ^= subkeys->y110; ciphertext[3] ^= subkeys->y111;
	S(ciphertext); L(ciphertext);

	/* Round 3 with subkey 3 */
	ciphertext[0] ^= subkeys->y200; ciphertext[1] ^= subkeys->y201; ciphertext[2] ^= subkeys->y210; ciphertext[3] ^= subkeys->y211;
	S(ciphertext); L(ciphertext);
	
	/* Round 4 with subkey 4 */
	ciphertext[0] ^= subkeys->y300; ciphertext[1] ^= subkeys->y301; ciphertext[2] ^= subkeys->y310; ciphertext[3] ^= subkeys->y311;
	S(ciphertext); L(ciphertext);
	
	/* Round 5 with subkey 5 */
	ciphertext[0] ^= subkeys->y400; ciphertext[1] ^= subkeys->y401; ciphertext[2] ^= subkeys->y410; ciphertext[3] ^= subkeys->y411;
	S(ciphertext); L(ciphertext);
	
	/* Round 6 with subkey 6 */
	ciphertext[0] ^= subkeys->y500; ciphertext[1] ^= subkeys->y501; ciphertext[2] ^= subkeys->y510; ciphertext[3] ^= subkeys->y511;
	S(ciphertext); L(ciphertext);
	
	/* Round 7 with subkey 7 */
	ciphertext[0] ^= subkeys->y600; ciphertext[1] ^= subkeys->y601; ciphertext[2] ^= subkeys->y610; ciphertext[3] ^= subkeys->y611;
	S(ciphertext); L(ciphertext);
	
	/* Round 8 with subkey 8 */
	ciphertext[0] ^= subkeys->y700; ciphertext[1] ^= subkeys->y701; ciphertext[2] ^= subkeys->y710; ciphertext[3] ^= subkeys->y711;
	S(ciphertext); L(ciphertext);
	
	/* Round 9 with subkey 9 */
	ciphertext[0] ^= subkeys->y800; ciphertext[1] ^= subkeys->y801; ciphertext[2] ^= subkeys->y810; ciphertext[3] ^= subkeys->y811;
	S(ciphertext); L(ciphertext);
	
	/* Round 10 with subkey 10 */
	ciphertext[0] ^= subkeys->y900; ciphertext[1] ^= subkeys->y901; ciphertext[2] ^= subkeys->y910; ciphertext[3] ^= subkeys->y911;

	changeEndian(&ciphertext[0]); changeEndian(&ciphertext[1]); changeEndian(&plaintext[2]); changeEndian(&plaintext[3]);
	
}

/**
 *
 *	\fn void grasshopper_decrypt(grasshopper_kds* subkeys, unsigned long * ciphertext, unsigned long * plaintext)
 *	\brief Decryption procedure
 *	\param[in] unsigned long * plaintext
 *	\param[in] unsigned long * ciphertext
 *	\param[in] grasshopper_kds * subkeys
 *	\return void
 *
 */
void grasshopper_decrypt(grasshopper_kds* subkeys, unsigned long * ciphertext, unsigned long * plaintext)
{
	changeEndian(&ciphertext[0]); changeEndian(&ciphertext[1]); changeEndian(&ciphertext[2]); changeEndian(&ciphertext[3]);

	/* Round 1 with subkey 10 */
	plaintext[0] = ciphertext[0] ^ subkeys->y900; plaintext[1] = ciphertext[1] ^ subkeys->y901; plaintext[2] = ciphertext[2] ^ subkeys->y910; plaintext[3] = ciphertext[3] ^ subkeys->y911;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 2 with subkey 9 */
	plaintext[0] ^= subkeys->y800; plaintext[1] ^= subkeys->y801; plaintext[2] ^= subkeys->y810; plaintext[3] ^= subkeys->y811;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 3 with subkey 8 */
	plaintext[0] ^= subkeys->y700; plaintext[1] ^= subkeys->y701; plaintext[2] ^= subkeys->y710; plaintext[3] ^= subkeys->y711;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 4 with subkey 7 */
	plaintext[0] ^= subkeys->y600; plaintext[1] ^= subkeys->y601; plaintext[2] ^= subkeys->y610; plaintext[3] ^= subkeys->y611;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 5 with subkey 6 */
	plaintext[0] ^= subkeys->y500; plaintext[1] ^= subkeys->y501; plaintext[2] ^= subkeys->y510; plaintext[3] ^= subkeys->y511;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 6 with subkey 5 */
	plaintext[0] ^= subkeys->y400; plaintext[1] ^= subkeys->y401; plaintext[2] ^= subkeys->y410; plaintext[3] ^= subkeys->y411;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 7 with subkey 4 */
	plaintext[0] ^= subkeys->y300; plaintext[1] ^= subkeys->y301; plaintext[2] ^= subkeys->y310; plaintext[3] ^= subkeys->y311;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 8 with subkey 3 */
	plaintext[0] ^= subkeys->y200; plaintext[1] ^= subkeys->y201; plaintext[2] ^= subkeys->y210; plaintext[3] ^= subkeys->y211;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 9 with subkey 2 */
	plaintext[0] ^= subkeys->y100; plaintext[1] ^= subkeys->y101; plaintext[2] ^= subkeys->y110; plaintext[3] ^= subkeys->y111;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 10 with subkey 1 */
	plaintext[0] ^= subkeys->y000; plaintext[1] ^= subkeys->y001; plaintext[2] ^= subkeys->y010; plaintext[3] ^= subkeys->y011;
	/*L_inv(plaintext); S_inv(plaintext);*/

	changeEndian(&plaintext[0]); changeEndian(&plaintext[1]); changeEndian(&plaintext[2]); changeEndian(&plaintext[3]);

}

/**
 *
 *	\fn void grasshopper_xor_ks(grasshopper_kds *ks, grasshopper_kds *out_ks, unsigned long *in, unsigned int len)
 *	\brief Apply grasshopper algorithm
 *	\param[in] unsigned int len
 *	\param[in] unsigned long * in
 *	\param[in] grasshopper_kds * out_ks
 *	\param[in] grasshopper_kds * ks
 *	\return void
 *
 */
void grasshopper_xor_ks(grasshopper_kds *ks, grasshopper_kds *out_ks, unsigned long *in, unsigned int len)
{
	unsigned long i;
	if (!len)
		return;

	out_ks->y000 = ks->y000 ^ in[0]; out_ks->y001 = ks->y001 ^ in[0]; out_ks->y010 = ks->y010 ^ in[0]; out_ks->y011 = ks->y011 ^ in[0];
	out_ks->y100 = ks->y100 ^ in[0]; out_ks->y101 = ks->y101 ^ in[0]; out_ks->y110 = ks->y110 ^ in[0]; out_ks->y111 = ks->y111 ^ in[0];
	out_ks->y200 = ks->y200 ^ in[0]; out_ks->y201 = ks->y201 ^ in[0]; out_ks->y210 = ks->y210 ^ in[0]; out_ks->y211 = ks->y211 ^ in[0];
	out_ks->y300 = ks->y300 ^ in[0]; out_ks->y301 = ks->y301 ^ in[0]; out_ks->y310 = ks->y310 ^ in[0]; out_ks->y311 = ks->y311 ^ in[0];
	out_ks->y400 = ks->y400 ^ in[0]; out_ks->y401 = ks->y401 ^ in[0]; out_ks->y410 = ks->y410 ^ in[0]; out_ks->y411 = ks->y411 ^ in[0];
	out_ks->y500 = ks->y500 ^ in[0]; out_ks->y501 = ks->y501 ^ in[0]; out_ks->y510 = ks->y510 ^ in[0]; out_ks->y511 = ks->y511 ^ in[0];
	out_ks->y600 = ks->y600 ^ in[0]; out_ks->y601 = ks->y601 ^ in[0]; out_ks->y610 = ks->y610 ^ in[0]; out_ks->y611 = ks->y611 ^ in[0];
	out_ks->y700 = ks->y700 ^ in[0]; out_ks->y701 = ks->y701 ^ in[0]; out_ks->y710 = ks->y710 ^ in[0]; out_ks->y711 = ks->y711 ^ in[0];
	out_ks->y800 = ks->y800 ^ in[0]; out_ks->y801 = ks->y801 ^ in[0]; out_ks->y810 = ks->y810 ^ in[0]; out_ks->y811 = ks->y811 ^ in[0];
	out_ks->y900 = ks->y900 ^ in[0]; out_ks->y901 = ks->y901 ^ in[0]; out_ks->y910 = ks->y910 ^ in[0]; out_ks->y911 = ks->y911 ^ in[0];
	for (i = 1; i < len; i++)
	{
		out_ks->y000 = ks->y000 ^ in[i]; out_ks->y001 = ks->y001 ^ in[0]; out_ks->y010 = ks->y010 ^ in[0]; out_ks->y011 = ks->y011 ^ in[0];
		out_ks->y100 = ks->y100 ^ in[i]; out_ks->y101 = ks->y101 ^ in[0]; out_ks->y110 = ks->y110 ^ in[0]; out_ks->y111 = ks->y111 ^ in[0];
		out_ks->y200 = ks->y200 ^ in[i]; out_ks->y201 = ks->y201 ^ in[0]; out_ks->y210 = ks->y210 ^ in[0]; out_ks->y211 = ks->y211 ^ in[0];
		out_ks->y300 = ks->y300 ^ in[i]; out_ks->y301 = ks->y301 ^ in[0]; out_ks->y310 = ks->y310 ^ in[0]; out_ks->y311 = ks->y311 ^ in[0];
		out_ks->y400 = ks->y400 ^ in[i]; out_ks->y401 = ks->y401 ^ in[0]; out_ks->y410 = ks->y410 ^ in[0]; out_ks->y411 = ks->y411 ^ in[0];
		out_ks->y500 = ks->y500 ^ in[i]; out_ks->y501 = ks->y501 ^ in[0]; out_ks->y510 = ks->y510 ^ in[0]; out_ks->y511 = ks->y511 ^ in[0];
		out_ks->y600 = ks->y600 ^ in[i]; out_ks->y601 = ks->y601 ^ in[0]; out_ks->y610 = ks->y610 ^ in[0]; out_ks->y611 = ks->y611 ^ in[0];
		out_ks->y700 = ks->y700 ^ in[i]; out_ks->y701 = ks->y701 ^ in[0]; out_ks->y710 = ks->y710 ^ in[0]; out_ks->y711 = ks->y711 ^ in[0];
		out_ks->y800 = ks->y800 ^ in[i]; out_ks->y801 = ks->y801 ^ in[0]; out_ks->y810 = ks->y810 ^ in[0]; out_ks->y811 = ks->y811 ^ in[0];
		out_ks->y900 = ks->y900 ^ in[i]; out_ks->y901 = ks->y901 ^ in[0]; out_ks->y910 = ks->y910 ^ in[0]; out_ks->y911 = ks->y911 ^ in[0];
	}
}

/**
 *
 *	\fn void changeEndian(unsigned long *ret)
 *	\brief Switch opposed bytes (lower byte to upper byte, ...) of unsigned long
 *	\param[in] unsigned long * ret
 *	\return void
 *
 */
void changeEndian(unsigned long *ret)
{
	unsigned long tmp = 0L;

	tmp  = (*ret & 0xFF) << 24;
	tmp |= (*ret & 0xFF00) << 8;
	tmp |= (*ret & 0xFF0000) >> 8;
	tmp |= (*ret & 0xFF000000) >> 24;
	*ret = tmp;

	return;
}
