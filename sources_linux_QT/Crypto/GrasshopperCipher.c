
/* Implementation of Gost Grasshopper Encryption Algorithm                         */
/* This program implements the test vectors set provided in the reference document */
/* provided by the Standartinform, Moscow, Russian Federation.                     */
/*                                                                                 */
/* Author: Eric Filiol                                                             */
/* This program is protected by the GPLv3                                          */
/***********************************************************************************/

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
void S(unsigned long long * block)
{
    unsigned long long valtmp;

    valtmp  = ((unsigned long long)(pi[(block[0] >> 56) & 0xFF]) << 56);
    valtmp |= ((unsigned long long)(pi[(block[0] >> 48) & 0xFF]) << 48);
    valtmp |= ((unsigned long long)(pi[(block[0] >> 40) & 0xFF]) << 40);
    valtmp |= ((unsigned long long)(pi[(block[0] >> 32) & 0xFF]) << 32);
    valtmp |= ((unsigned long long)(pi[(block[0] >> 24) & 0xFF]) << 24);
    valtmp |= ((unsigned long long)(pi[(block[0] >> 16) & 0xFF]) << 16);
    valtmp |= ((unsigned long long)(pi[(block[0] >>  8) & 0xFF]) <<  8);
    valtmp |=  (unsigned long long)(pi[block[0] & 0xFF]);

    block[0] = valtmp;

    valtmp  = ((unsigned long long)(pi[(block[1] >> 56) & 0xFF]) << 56);
    valtmp |= ((unsigned long long)(pi[(block[1] >> 48) & 0xFF]) << 48);
    valtmp |= ((unsigned long long)(pi[(block[1] >> 40) & 0xFF]) << 40);
    valtmp |= ((unsigned long long)(pi[(block[1] >> 32) & 0xFF]) << 32);
    valtmp |= ((unsigned long long)(pi[(block[1] >> 24) & 0xFF]) << 24);
    valtmp |= ((unsigned long long)(pi[(block[1] >> 16) & 0xFF]) << 16);
    valtmp |= ((unsigned long long)(pi[(block[1] >>  8) & 0xFF]) <<  8);
    valtmp |=  (unsigned long long)(pi[block[1] & 0xFF]);

    block[1] = valtmp;
}

/* Inverse of S conversion function                */
void S_inv(unsigned long long * block)
{
    unsigned long long valtmp;

    valtmp  = ((unsigned long long)(pi_inv[(block[0] >> 56) & 0xFF]) << 56);
    valtmp |= ((unsigned long long)(pi_inv[(block[0] >> 48) & 0xFF]) << 48);
    valtmp |= ((unsigned long long)(pi_inv[(block[0] >> 40) & 0xFF]) << 40);
    valtmp |= ((unsigned long long)(pi_inv[(block[0] >> 32) & 0xFF]) << 32);
    valtmp |= ((unsigned long long)(pi_inv[(block[0] >> 24) & 0xFF]) << 24);
    valtmp |= ((unsigned long long)(pi_inv[(block[0] >> 16) & 0xFF]) << 16);
    valtmp |= ((unsigned long long)(pi_inv[(block[0] >>  8) & 0xFF]) <<  8);
    valtmp |=  (unsigned long long)(pi_inv[block[0] & 0xFF]);

    block[0] = valtmp;

    valtmp  = ((unsigned long long)(pi_inv[(block[1] >> 56) & 0xFF]) << 56);
    valtmp |= ((unsigned long long)(pi_inv[(block[1] >> 48) & 0xFF]) << 48);
    valtmp |= ((unsigned long long)(pi_inv[(block[1] >> 40) & 0xFF]) << 40);
    valtmp |= ((unsigned long long)(pi_inv[(block[1] >> 32) & 0xFF]) << 32);
    valtmp |= ((unsigned long long)(pi_inv[(block[1] >> 24) & 0xFF]) << 24);
    valtmp |= ((unsigned long long)(pi_inv[(block[1] >> 16) & 0xFF]) << 16);
    valtmp |= ((unsigned long long)(pi_inv[(block[1] >>  8) & 0xFF]) <<  8);
    valtmp |=  (unsigned long long)(pi_inv[block[1] & 0xFF]);

    block[1] = valtmp;
}

inline unsigned char ll(unsigned long long *block)
{
	
	return (mult_mod_poly[4][(block[0] >> 56) & 0xFF] ^
		    mult_mod_poly[2][(block[0] >> 48) & 0xFF] ^
			mult_mod_poly[3][(block[0] >> 40) & 0xFF] ^
			mult_mod_poly[1][(block[0] >> 32) & 0xFF] ^
			mult_mod_poly[6][(block[0] >> 24) & 0xFF] ^
			mult_mod_poly[5][(block[0] >> 16) & 0xFF] ^
			mult_mod_poly[0][(block[0] >>  8) & 0xFF] ^
			mult_mod_poly[7][(block[0] >>  0) & 0xFF] ^
			mult_mod_poly[0][(block[1] >> 56) & 0xFF] ^
			mult_mod_poly[5][(block[1] >> 48) & 0xFF] ^
			mult_mod_poly[6][(block[1] >> 40) & 0xFF] ^
			mult_mod_poly[1][(block[1] >> 32) & 0xFF] ^
			mult_mod_poly[3][(block[1] >> 24) & 0xFF] ^
			mult_mod_poly[2][(block[1] >> 16) & 0xFF] ^
			mult_mod_poly[4][(block[1] >>  8) & 0xFF] ^
			mult_mod_poly[0][(block[1] >>  0) & 0xFF] );

}

/* R conversion function                            */
inline void R(unsigned long long * block)
{
	unsigned long long valtmp[2];
	unsigned char val;

	valtmp[0] = block[0];
	valtmp[1] = block[1];
	val = ll(valtmp);

	block[1] >>= 8;
	block[1]  |= (((block[0]) & 0xFF) << 56);

	block[0] >>= 8;
	block[0]  |= ((unsigned long long)(val) << 56);
}

/* Inverse of R conversion function                 */
inline void R_inv(unsigned long long * block)
{
	unsigned long long valtmp[2];
	unsigned char val;

	valtmp[0]  = (block[0] << 8);
	valtmp[0] |= ((block[1] >> 56) & 0xFF);
	valtmp[1]  = (block[1] << 8);
	valtmp[1] |= ((block[0] >> 56) & 0xFF);

	val = ll(valtmp);

	block[0]  = valtmp[0];
	block[1]  = (valtmp[1] & 0xFFFFFFFFFFFFFF00L);
	block[1] |= val;
}

/* L conversion function (16 iteration of R)        */
inline void L(unsigned long long * block)
{
	R(block); R(block); R(block); R(block);
	R(block); R(block); R(block); R(block);
	R(block); R(block); R(block); R(block);
	R(block); R(block); R(block); R(block);
}

/* Inverse of L conversion function (16 iteration of R_inv) */
inline void L_inv(unsigned long long * block)
{
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
	R_inv(block); R_inv(block); R_inv(block); R_inv(block);
}

/* F conversion function (used in key setup)        */
inline void F(unsigned long long * k, unsigned long long * block1, unsigned long long * block0)
{
	unsigned long long tmpblock[2];

	/* Bitwise xor of k with block2 */
	tmpblock[0] = (block1[0] ^ k[0]);
	tmpblock[1] = (block1[1] ^ k[1]);

	/* Apply S conversion function  */
	S(tmpblock);

	/* Apply L conversion function  */
	L(tmpblock);

	/* Add a0 to the the result     */
	tmpblock[0] ^= block0[0];
	tmpblock[1] ^= block0[1];

	/* Update the results           */
	block0[0] = block1[0];
	block0[1] = block1[1];

	block1[0] = tmpblock[0];
	block1[1] = tmpblock[1];
}

/****************************************************/
/****************************************************/
/* The three core functions                         */
/****************************************************/
/****************************************************/

/****************************************************/
/* Key setup procedure                              */
/****************************************************/
void grasshopper_set_key(unsigned long long * master_key, grasshopper_kds *subkeys)
{

	unsigned long long c_values[32][2], blocktmp[2], blocktmp2[2];
	unsigned char i;

	/* Compute the C_i values       */
	for(i = 0; i < 32; i++)
	{
		blocktmp[0] = 0L;
		blocktmp[1] = (unsigned long long)(i+1);
		L(blocktmp);
		c_values[i][0] = blocktmp[0]; c_values[i][1] = blocktmp[1];
	}

	/* Change endian */

	/* Compute subkeys K_1 and K_2  */
	subkeys->x00 =  master_key[0]; subkeys->x01 =  master_key[1];
	subkeys->x10 =  master_key[2]; subkeys->x11 =  master_key[3];

    changeEndian(&subkeys->x00); changeEndian(&subkeys->x01);
    changeEndian(&subkeys->x10); changeEndian(&subkeys->x11);

	/* Compute subkeys K_3 and K_4  */
	blocktmp[0] = subkeys->x00; blocktmp[1] = subkeys->x01;
	blocktmp2[0] = subkeys->x10; blocktmp2[1] = subkeys->x11;
	F(c_values[0], blocktmp, blocktmp2); F(c_values[1], blocktmp, blocktmp2);
	F(c_values[2], blocktmp, blocktmp2); F(c_values[3], blocktmp, blocktmp2);
	F(c_values[4], blocktmp, blocktmp2); F(c_values[5], blocktmp, blocktmp2);
	F(c_values[6], blocktmp, blocktmp2); F(c_values[7], blocktmp, blocktmp2);

	subkeys->x20 = blocktmp[0]; subkeys->x21 = blocktmp[1];
	subkeys->x30 = blocktmp2[0]; subkeys->x31 = blocktmp2[1];

	/* Compute subkeys K_5 and K_6  */
	F(c_values[ 8], blocktmp, blocktmp2); F(c_values[ 9], blocktmp, blocktmp2);
	F(c_values[10], blocktmp, blocktmp2); F(c_values[11], blocktmp, blocktmp2);
	F(c_values[12], blocktmp, blocktmp2); F(c_values[13], blocktmp, blocktmp2);
	F(c_values[14], blocktmp, blocktmp2); F(c_values[15], blocktmp, blocktmp2);

	subkeys->x40 = blocktmp[0]; subkeys->x41 = blocktmp[1];
	subkeys->x50 = blocktmp2[0]; subkeys->x51 = blocktmp2[1];

	/* Compute subkeys K_7 and K_8  */
	F(c_values[16], blocktmp, blocktmp2); F(c_values[17], blocktmp, blocktmp2);
	F(c_values[18], blocktmp, blocktmp2); F(c_values[19], blocktmp, blocktmp2);
	F(c_values[20], blocktmp, blocktmp2); F(c_values[21], blocktmp, blocktmp2);
	F(c_values[22], blocktmp, blocktmp2); F(c_values[23], blocktmp, blocktmp2);

	subkeys->x60 = blocktmp[0]; subkeys->x61 = blocktmp[1];
	subkeys->x70 = blocktmp2[0]; subkeys->x71 = blocktmp2[1];

	/* Compute subkeys K_9 and K_{10} */
	F(c_values[24], blocktmp, blocktmp2); F(c_values[25], blocktmp, blocktmp2);
	F(c_values[26], blocktmp, blocktmp2); F(c_values[27], blocktmp, blocktmp2);
	F(c_values[28], blocktmp, blocktmp2); F(c_values[29], blocktmp, blocktmp2);
	F(c_values[30], blocktmp, blocktmp2); F(c_values[31], blocktmp, blocktmp2);

	subkeys->x80 = blocktmp[0]; subkeys->x81 = blocktmp[1];
	subkeys->x90 = blocktmp2[0]; subkeys->x91 = blocktmp2[1];

}

/****************************************************/
/* Encryption procedure                             */
/****************************************************/
void grasshopper_encrypt(grasshopper_kds *subkeys, unsigned long long * plaintext, unsigned long long * ciphertext)
{
	/* Round 1 with subkey 1 */

	changeEndian(&plaintext[0]); changeEndian(&plaintext[1]);

	ciphertext[0] = plaintext[0] ^ subkeys->x00; ciphertext[1] = plaintext[1] ^ subkeys->x01;
	S(ciphertext); L(ciphertext);

	/* Round 2 with subkey 2 */
	ciphertext[0] ^= subkeys->x10; ciphertext[1] ^= subkeys->x11;
	S(ciphertext); L(ciphertext);

	/* Round 3 with subkey 3 */
    ciphertext[0] ^= subkeys->x20; ciphertext[1] ^= subkeys->x21;
	S(ciphertext); L(ciphertext);
	
	/* Round 4 with subkey 4 */
	ciphertext[0] ^= subkeys->x30; ciphertext[1] ^= subkeys->x31;
	S(ciphertext); L(ciphertext);
	
	/* Round 5 with subkey 5 */
	ciphertext[0] ^= subkeys->x40; ciphertext[1] ^= subkeys->x41;
	S(ciphertext); L(ciphertext);
	
	/* Round 6 with subkey 6 */
	ciphertext[0] ^= subkeys->x50; ciphertext[1] ^= subkeys->x51;
	S(ciphertext); L(ciphertext);
	
	/* Round 7 with subkey 7 */
	ciphertext[0] ^= subkeys->x60; ciphertext[1] ^= subkeys->x61;
	S(ciphertext); L(ciphertext);
	
	/* Round 8 with subkey 8 */
	ciphertext[0] ^= subkeys->x70; ciphertext[1] ^= subkeys->x71;
	S(ciphertext); L(ciphertext);
	
	/* Round 9 with subkey 9 */
	ciphertext[0] ^= subkeys->x80; ciphertext[1] ^= subkeys->x81;
	S(ciphertext); L(ciphertext);
	
	/* Round 10 with subkey 10 */
	ciphertext[0] ^= subkeys->x90; ciphertext[1] ^= subkeys->x91;

	changeEndian(&ciphertext[0]); changeEndian(&ciphertext[1]);
	
}

/****************************************************/
/* Decryption procedure                             */
/****************************************************/
void grasshopper_decrypt(grasshopper_kds* subkeys, unsigned long long * ciphertext, unsigned long long * plaintext)
{
	changeEndian(&ciphertext[0]); changeEndian(&ciphertext[1]);

	/* Round 1 with subkey 10 */
	plaintext[0] = ciphertext[0] ^ subkeys->x90; plaintext[1] = ciphertext[1] ^ subkeys->x91;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 2 with subkey 9 */
	plaintext[0] ^= subkeys->x80; plaintext[1] ^= subkeys->x81;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 3 with subkey 8 */
	plaintext[0] ^= subkeys->x70; plaintext[1] ^= subkeys->x71;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 4 with subkey 7 */
	plaintext[0] ^= subkeys->x60; plaintext[1] ^= subkeys->x61;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 5 with subkey 6 */
	plaintext[0] ^= subkeys->x50; plaintext[1] ^= subkeys->x51;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 6 with subkey 5 */
	plaintext[0] ^= subkeys->x40; plaintext[1] ^= subkeys->x41;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 7 with subkey 4 */
	plaintext[0] ^= subkeys->x30; plaintext[1] ^= subkeys->x31;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 8 with subkey 3 */
	plaintext[0] ^= subkeys->x20; plaintext[1] ^= subkeys->x21;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 9 with subkey 2 */
	plaintext[0] ^= subkeys->x10; plaintext[1] ^= subkeys->x11;
	L_inv(plaintext); S_inv(plaintext);

	/* Round 10 with subkey 1 */
	plaintext[0] ^= subkeys->x00; plaintext[1] ^= subkeys->x01;
	/*L_inv(plaintext); S_inv(plaintext);*/

	changeEndian(&plaintext[0]); changeEndian(&plaintext[1]);
}

void grasshopper_xor_ks(grasshopper_kds *ks, grasshopper_kds *out_ks, unsigned long long *in, unsigned int len)
{
	unsigned long long i;
	if (!len)
		return;

	out_ks->x00 = ks->x00 ^ in[0];out_ks->x01 = ks->x90 ^ in[0];
	out_ks->x10 = ks->x10 ^ in[0];out_ks->x11 = ks->x90 ^ in[0];
	out_ks->x20 = ks->x20 ^ in[0];out_ks->x21 = ks->x90 ^ in[0];
	out_ks->x30 = ks->x30 ^ in[0];out_ks->x31 = ks->x90 ^ in[0];
	out_ks->x40 = ks->x40 ^ in[0];out_ks->x41 = ks->x90 ^ in[0];
	out_ks->x50 = ks->x50 ^ in[0];out_ks->x51 = ks->x90 ^ in[0];
	out_ks->x60 = ks->x60 ^ in[0];out_ks->x61 = ks->x90 ^ in[0];
	out_ks->x70 = ks->x70 ^ in[0];out_ks->x71 = ks->x90 ^ in[0];
	out_ks->x80 = ks->x80 ^ in[0];out_ks->x81 = ks->x90 ^ in[0];
	out_ks->x90 = ks->x90 ^ in[0];out_ks->x91 = ks->x90 ^ in[0];
	for (i = 1; i < len; i++)
	{
		out_ks->x00 = ks->x00 ^ in[i];out_ks->x01 = ks->x90 ^ in[i];
		out_ks->x10 = ks->x10 ^ in[i];out_ks->x11 = ks->x90 ^ in[i];
		out_ks->x20 = ks->x20 ^ in[i];out_ks->x21 = ks->x90 ^ in[i];
		out_ks->x30 = ks->x30 ^ in[i];out_ks->x31 = ks->x90 ^ in[i];
		out_ks->x40 = ks->x40 ^ in[i];out_ks->x41 = ks->x90 ^ in[i];
		out_ks->x50 = ks->x50 ^ in[i];out_ks->x51 = ks->x90 ^ in[i];
		out_ks->x60 = ks->x60 ^ in[i];out_ks->x61 = ks->x90 ^ in[i];
		out_ks->x70 = ks->x70 ^ in[i];out_ks->x71 = ks->x90 ^ in[i];
		out_ks->x80 = ks->x80 ^ in[i];out_ks->x81 = ks->x90 ^ in[i];
		out_ks->x90 = ks->x90 ^ in[i];out_ks->x91 = ks->x90 ^ in[i];
	}
}

void changeEndian(unsigned long long *ret)
{
	unsigned long long tmp1 = 0LL;

	tmp1  = (*ret & 0x00000000000000ff) << 56;
	tmp1 |= (*ret & 0x000000000000ff00) << 40;
	tmp1 |= (*ret & 0x0000000000ff0000) << 24;
	tmp1 |= (*ret & 0x00000000ff000000) << 8;

	tmp1 |= (*ret & 0x000000ff00000000) >> 8;
	tmp1 |= (*ret & 0x0000ff0000000000) >> 24;
	tmp1 |= (*ret & 0x00ff000000000000) >> 40;
	tmp1 |= (*ret & 0xff00000000000000) >> 56;
	*ret = tmp1;
}
