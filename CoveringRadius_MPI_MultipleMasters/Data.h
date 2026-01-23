#include <iostream>
#include <cstdlib>
#include <cstdio>

#ifndef ERRORQ
#define ERRORQ(expr) \
        if ((expr)) { \
		FILE *fran=NULL;\
		fran= fopen("error.txt", "a");\
		fprintf(fran,"ERROR %s: line %d: assertion failed: " \
			"(%s)\n",__FILE__,__LINE__,#expr); \
			int i=fclose(fran); \
			exit(EXIT_FAILURE);\
			int t;\
           t = scanf("%d", &t);\
	while(true)\
	{\
		printf("ERROR");\
        	        }\
			exit(EXIT_FAILURE);\
	}
#endif

typedef unsigned long long int* dynamic_row_short;
typedef struct  _dmatlu_type dynamic_mat_short;

struct _dmatlu_type { // Structure for dynamic generator matrix (unsigned long long int)
	int n, k, nreal, kreal, q, mem = 0, num = 0;
	dynamic_row_short* a;
};

typedef unsigned char* drow;

/// <summary>
/// Definition of dynamic structure describing generator matrix of a linear code saved as dynamic array of char elements
/// </summary>
typedef struct _dmat_type dmat_type;

struct _dmat_type { // Structure for dynamic generator matrix (char)
	int n = 0, k = 0, nreal = 0, kreal = 0, q = 0, mem = 0;
	char name[50] = {};
	drow* a = NULL;
};

struct mat_type {
	int n = 0, k = 0,  q = 0,  num=0;
	unsigned char a[100][100];} matrix;


 dmat_type bitsCharCF;
 dynamic_mat_short bits;
 //dynamic_mat_short matrix;
 //dynamic_mat_short matrixH;

 // Allocating memory for a dynamic row of unsigned long long int elements
 dynamic_row_short drow_short_new(int size) {
	 dynamic_row_short r;
	 int size2 = size + 1;
	 r = (unsigned long long int*)malloc((size2) * sizeof(unsigned long long int));
	 return &(r[0]);
 }

 // Free the memory for a dynamic row
 void drow_short_free(dynamic_row_short r) {
	 free(&(r[0]));
	 r = NULL;
 }

 // allocating memory for dynamic generator matrix (unsigned long long int)
 dynamic_mat_short dmat_short_newh(dynamic_mat_short& c, int k, int n, int q) {
	 int i;
	 c.n = n;
	 c.k = k;
	 c.nreal = n;
	 c.kreal = k;
	 c.q = q;
	 int m1 = k + 1;
	 c.a = (dynamic_row_short*)malloc((m1) * sizeof(dynamic_row_short));
	 //if (c.a == NULL) { if (MM(RD)) c.a = (drowlu*)malloc((m1) * sizeof(drowlu)); }

	 ERRORQ(c.a == NULL);
	 for (i = 0; i < m1; i++) {
		 c.a[i] = drow_short_new(n);
	 }
	 c.mem = 1;
	 return c;
 }

 //free the memory of dynamic matrix
 void dmat_short_free(dynamic_mat_short& c) {
	 int i;

	 if (c.mem != 0) {
		 for (i = 0; i < c.kreal + 1; i++) {
			 drow_short_free(c.a[i]);
		 }
	 }
	 free(c.a);
	 c.mem = 0;

	 return;
 }


 // main function for allocating dynamic matrix: allocate new memory only if there is need
 dynamic_mat_short dmat_short_new(dynamic_mat_short& c, int k, int n, int q) {
	 c.k = k;
	 c.n = n;

	 if (1 == c.mem)
	 {
		 if ((c.nreal < n) || (c.kreal < k))
		 {
			 //WORNQ(c.nreal < n);
			 //WORNQ(c.mreal < m);
			 dmat_short_free(c);
			 c = dmat_short_newh(c, k, n, q);
		 }
	 };
	 if (0 == c.mem)
	 {
		 c.mem = 1;
		 c = dmat_short_newh(c, k, n, q);
	 }
	 return c;
 }


 // Allocating memory for a dynamic row of char elements
 drow drow_new(int size) {
	 drow r;
	 int i = size + 2;
	 int j = sizeof(unsigned char);
	 int ij = i * j;
	 r = (unsigned char*)malloc(ij);

	 //if (r == NULL) { if (MM(RD)) r = (unsigned char*)malloc(ij); }

	 ERRORQ(r == NULL);

	 return &(r[0]);
 }

 // Free memory for dynamic row (char)
 void drow_free(drow& r) {
	 if (NULL != r)
	 {
		 free(&(r[0]));
		 ERRORQ(NULL == &(r[-1]));
		 r = NULL;
	 }
 }

 // Allocating memory for dynamic generator matrix (char)
 dmat_type dmat_newh(dmat_type& c, int k, int n, int q) {
	 //dmat_type c;
	 int i;
	 c.n = n;
	 c.k = k;
	 c.nreal = n;
	 c.kreal = k;
	 c.q = q;
	 int m1 = k + 1;
	 c.a = (drow*)malloc((m1) * sizeof(drow));
	 //if (c.a == NULL) { if (MM(RD)) c.a = (drow*)malloc((m1) * sizeof(drow)); }
	 ERRORQ(c.a == NULL);
	 for (i = 0; i < k + 1; i++) {
		 c.a[i] = drow_new(n);
	 }
	 c.mem = 1;
	 return c;
 }

 void dmat_free(dmat_type& c) {
	 int i;
	 if (!((1 == c.mem) || (0 == c.mem)))
		 ERRORQ(!((1 == c.mem) || (0 == c.mem)))

		 if (0 == c.mem)
		 {
			 //WORNQ(0 == c.mem);
			 return;
		 }

	 for (i = 0; i < c.kreal + 1; i++) {
		 drow_free(c.a[i]);
	 }

	 free(c.a);
	 c.mem = 0;
	 ERRORQ(NULL == c.a);

	 return;
 }

 // Main function for allocating matrix (char)
 dmat_type dmat_new(dmat_type& c, int k, int n, int q) {
	 if (!((1 == c.mem) || (0 == c.mem))) {}
	 //ERRORQ(!((1 == c.mem) || (0 == c.mem)))
	 c.k = k;
	 c.q = q;
	 c.n = n;
	 if (1 == c.mem)
	 {
		 if ((c.nreal < n) || (c.kreal < k))
		 {
			 //WORNQ(c.nreal < n);
			 //WORNQ(c.mreal < m);
			 dmat_free(c);
			 c = dmat_newh(c, k, n, q);
		 }
	 };
	 if (0 == c.mem)
	 {
		 c.mem = 1;
		 c = dmat_newh(c, k, n, q);
	 }
	 return c;
 }

 // Free the memory of dynamic matrix (char)

 dmat_type make_dmat(int m, int n, int q) {
	 dmat_type c;
	 c.mem = 0;
	 dmat_new(c, m, n, q);
	 return c;

 }


const unsigned long long int one = 1;

// Generator matrix to bit representation (GF2)
void num_to_coef_gf2(int& k, int& n, int m) { // Multiple of 512
	 // printf(" Generator matrix to bit representation (GF2)\n");
	 int c = ((n - 1) / 64) + 1;
	 for (int row = 0; row < k; row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 <= n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 if (matrix.a[row + 1][el * 64 + shift] == 1) {
						 bits.a[row][el] = bits.a[row][el] | (one << (63 - shift));
					 }
				 }
			 }
		 }
	 }
 }

// Generator matrix to bit representation (GF4)
 void num_to_coef_gf4(int& k, int& n, int m) {
	 int c = (((n - 1) / 64) + 1);
	 for (int row = 0; row < 2 * k; row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 <= n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 if (matrix.a[row + 1][el * 64 + shift] == 1) {
						 bits.a[row][el] = bits.a[row][el] | (one << (63 - shift));
					 }
					 else if (matrix.a[row + 1][el * 64 + shift] == 2) {
						 bits.a[row][el + c] = bits.a[row][el + c] | (one << (63 - shift));
					 }
					 else if (matrix.a[row + 1][el * 64 + shift] == 3) {
						 bits.a[row][el + c] = bits.a[row][el + c] | (one << (63 - shift));
						 bits.a[row][el] = bits.a[row][el] | (one << (63 - shift));
					 }
				 }
			 }
		 }
	 }
 }

 // Generator matrix to bit representation (GF8)
 void num_to_coef_gf8(int& k, int& n, int m) {
	 int c = (((n - 1) / 64) + 1);
	 for (int row = 0; row < 3 * k; row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 <= n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 int t = (int)matrix.a[row + 1][el * 64 + shift];
					 int ost = t % 2; // ^0
					 if (ost) { bits.a[row][el] = bits.a[row][el] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^1
					 if (ost) { bits.a[row][el + 1 * c] = bits.a[row][el + 1 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^2
					 if (ost) { bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | (one << (63 - shift)); }
				 }
			 }
		 }
	 }
 }

 // Generator matrix to bit representation (GF16)
 void num_to_coef_gf16(int& k, int& n, int m) {
	 int c = (((n - 1) / 64) + 1);
	 for (int row = 0; row < (4 * k); row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 <= n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 int t = (int)matrix.a[row + 1][el * 64 + shift];
					 int ost = t % 2; //power 0
					 if (ost) { bits.a[row][el] = bits.a[row][el] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; //power 1
					 if (ost) { bits.a[row][el + 1 * c] = bits.a[row][el + 1 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; //power 2
					 if (ost) { bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; //power 3
					 if (ost) { bits.a[row][el + 3 * c] = bits.a[row][el + 3 * c] | (one << (63 - shift)); }
				 }
			 }
		 }
	 }
 }

 // Generator matrix to coef (char)(GF2)
 void num_to_coef_char2(int k, int n, int m) {
	 int totalRows = m * k;
	 char oneChar = 1;
	 for (int row = 0; row < totalRows; row++) {
		 for (int col = 0; col < n; col++) {
			 bitsCharCF.a[row][col] = matrix.a[row + 1][col];
		 }
	 }
 }
 
// Generator matrix to coef (char)(GF25)
 void num_to_coef_char25(int& k, int& n, int m) {
	 //printf(" Generator matrix to coef (char)(GF25)\n");
	 int totalRows = m * k;
	 char oneChar = 1;
	 for (int row = 0; row < totalRows; row++) {
		 for (int col = 0; col < n; col++) {
			 int temp = (int)matrix.a[row + 1][col];
			 bitsCharCF.a[row][col] = temp % 5; // ^0
			 temp = temp / 5;
			 bitsCharCF.a[row][col + n] = temp % 5; // ^1
		 }
	 }
 }

 // Generator matrix to coef (char)(GF49)
 void num_to_coef_char49(int& k, int& n, int m) {
	 int totalRows = m * k;
	 char oneChar = 1;
	 for (int row = 0; row < totalRows; row++) {
		 for (int col = 0; col < n; col++) {
			 //bits[row][col] = matrix[row + 1][col];
			 int temp = (int)matrix.a[row + 1][col];
			 bitsCharCF.a[row][col] = temp % 7; // ^0
			 temp = temp / 7;
			 bitsCharCF.a[row][col + n] = temp % 7; // ^1
		 }
	 }
 }
 
 // Generator matrix to bit representation (GF32)
 void num_to_coef_gf32(int& k, int& n, int m) {
	 int c = (((n - 1) / 64) + 1);
	 for (int row = 0; row < (5 * k); row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 <= n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 int t = (int)matrix.a[row + 1][el * 64 + shift];
					 int ost = t % 2; // ^0
					 if (ost) { bits.a[row][el] = bits.a[row][el] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^1
					 if (ost) { bits.a[row][el + 1 * c] = bits.a[row][el + 1 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^2
					 if (ost) { bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^3
					 if (ost) { bits.a[row][el + 3 * c] = bits.a[row][el + 3 * c] | (one << (63 - shift)); }


					 t = t / 2;
					 ost = t % 2; // ^4
					 if (ost) { bits.a[row][el + 4 * c] = bits.a[row][el + 4 * c] | (one << (63 - shift)); }
				 }
			 }
		 }
	 }
 }

 // Generator matrix to bit representation (GF64)
  void num_to_coef_gf64(int& k, int& n, int m) {
	 int c = (((n - 1) / 64) + 1);
	 for (int row = 0; row < (6 * k); row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 <= n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 int t = (int)matrix.a[row + 1][el * 64 + shift];
					 int ost = t % 2; // ^0
					 if (ost) { bits.a[row][el] = bits.a[row][el] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^1
					 if (ost) { bits.a[row][el + 1 * c] = bits.a[row][el + 1 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^2
					 if (ost) { bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^3
					 if (ost) { bits.a[row][el + 3 * c] = bits.a[row][el + 3 * c] | (one << (63 - shift)); }


					 t = t / 2;
					 ost = t % 2; // ^4
					 if (ost) { bits.a[row][el + 4 * c] = bits.a[row][el + 4 * c] | (one << (63 - shift)); }

					 t = t / 2;
					 ost = t % 2; // ^5
					 if (ost) { bits.a[row][el + 5 * c] = bits.a[row][el + 5 * c] | (one << (63 - shift)); }
				 }
			 }
		 }
	 }
 }

 // Generator matrix to bit representation (GF27)
 void num_to_coef_gf27(int k, int n, int m) {
	 //printf(" Generator matrix to bit representation (GF27)\n");
	 int c = (((n - 1) / 64) + 1);
	 unsigned long long int m_element = 0;
	 for (int row = 0; row < 3 * k; row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 < n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 unsigned long long int  n = 0;
					 unsigned long long int  t = 0;
					 unsigned long long int ost = 0;

					 //^0
					 m_element = matrix.a[row + 1][el * 64 + shift];
					 t = m_element / 3;
					 ost = m_element - t * 3;
					 if (ost == 0) {
						 bits.a[row][el] = bits.a[row][el] | (one << (63 - shift));
						 bits.a[row][el + 4 * c] = bits.a[row][el + 4 * c] | (one << (63 - shift));
					 }
					 else {
						 unsigned long long two = (2 & ost) >> 1;
						 bits.a[row][el + 4 * c] = bits.a[row][el + 4 * c] | ((one & ost) << (63 - shift));
						 bits.a[row][el] = bits.a[row][el] | ((two) << (63 - shift));
					 }
					 // ^1
					 m_element = t;
					 t = m_element / 3;
					 ost = m_element - t * 3;
					 if (ost == 0) {
						 bits.a[row][el + c] = bits.a[row][el + c] | (one << (63 - shift));
						 bits.a[row][el + 5 * c] = bits.a[row][el + 5 * c] | (one << (63 - shift));
					 }
					 else {
						 unsigned long long two = (2 & ost) >> 1;
						 bits.a[row][el + 5 * c] = bits.a[row][el + 5 * c] | ((one & ost) << (63 - shift));
						 bits.a[row][el + c] = bits.a[row][el + c] | ((two) << (63 - shift));
					 }
					 // ^2
					 m_element = t;
					 t = m_element / 3;
					 ost = m_element - t * 3;
					 if (ost == 0) {
						 bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | (one << (63 - shift));
						 bits.a[row][el + 6 * c] = bits.a[row][el + 6 * c] | (one << (63 - shift));
					 }
					 else {
						 unsigned long long two = (2 & ost) >> 1;
						 bits.a[row][el + 6 * c] = bits.a[row][el + 6 * c] | ((one & ost) << (63 - shift));
						 bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | ((two) << (63 - shift));
					 }
				 }
			 }
		 }
	 }
 }


 /* for 512:
 |    0 | 1 | 2 | 3 | 4 | 5 | 6 | 7   |||    0 | 1 | 2 | 3 | 4 | 5 | 6 | 7   |||    0 | 1 | 2 | 3 | 4 | 5 | 6 | 7    |||    0 | 1 | 2 | 3 | 4 | 5 | 6 | 7    |
 | 1st element, 1 bit, stored in 8x64 ||| 2nd element, 1 bit, stored in 8x64 ||| 1st element, 2 bits, stored in 8x64 ||| 2nd element, 2 bits, stored in 8x64 |
  */

 // Generator matrix to bit representation (GF9)
 void num_to_coef_gf9(int k, int n, int m) {
	 int c = (((n - 1) / 64) + 1);// c = 8 for 512;
	 unsigned long long int m_element = 0;
	 for (int row = 0; row < 2 * k; row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 < n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 unsigned long long int  n = 0;
					 unsigned long long int  t = 0;
					 unsigned long long int ost = 0;
					 m_element = matrix.a[row + 1][el * 64 + shift];
					 // ^0
					 t = m_element / 3;
					 ost = m_element - t * 3;
					 if (ost == 0) {
						 bits.a[row][el] = bits.a[row][el] | (one << (63 - shift));
						 bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | (one << (63 - shift)); // za 9 ->element + 16
					 }
					 else {
						 unsigned long long two = (2 & ost) >> 1;
						 bits.a[row][el + 2 * c] = bits.a[row][el + 2 * c] | ((one & ost) << (63 - shift));
						 bits.a[row][el] = bits.a[row][el] | ((two) << (63 - shift));
					 }

					 // ^1
					 m_element = t;
					 t = m_element / 3;
					 ost = m_element - t * 3;
					 if (ost == 0) {
						 bits.a[row][el + c] = bits.a[row][el + c] | (one << (63 - shift));
						 bits.a[row][el + 3 * c] = bits.a[row][el + 3 * c] | (one << (63 - shift));
					 }
					 else {
						 unsigned long long two = (2 & ost) >> 1;
						 bits.a[row][el + 3 * c] = bits.a[row][el + 3 * c] | ((one & ost) << (63 - shift));
						 bits.a[row][el + c] = bits.a[row][el + c] | ((two) << (63 - shift));
					 }
				 }
			 }
		 }
	 }
 }

 // Generator matrix to bit representation (GF3)
 void num_to_coef_gf3(int k, int n, int m) {
	 int c = (((n - 1) / 64) + 1);
	 int m_element = 0;
	 for (int row = 0; row < k; row++) {
		 for (int el = 0; el < c; el++) {
			 if ((el) * 64 < n) {
				 for (int shift = 0; shift < 64; shift++) {
					 if (el * 64 + shift > (n - 1)) { break; }
					 unsigned long long int  n = 0;
					 unsigned long long int  t = 0;
					 unsigned long long int ost = 0;
					 m_element = (int)matrix.a[row + 1][el * 64 + shift];
					 t = m_element / 3;
					 ost = m_element - t * 3;

					 // element of the field GF3 is represented in 2 bits

					 if (ost == 0) {
						 bits.a[row][el] = bits.a[row][el] | (one << (63 - shift));
						 bits.a[row][el + c] = bits.a[row][el + c] | (one << (63 - shift));
					 }
					 else {
						 unsigned long long two = (2 & ost) >> 1;
						 bits.a[row][el + c] = bits.a[row][el + c] | ((one & ost) << (63 - shift));
						 bits.a[row][el] = bits.a[row][el] | ((two) << (63 - shift));
					 }
				 }
			 }
		 }
	 }
 }


 int charStriingToInt(char* number, int len) {
	 // For reading decimal numbers >10
	 int d = 1, t = 0, res = 0;
	 for (int i = len - 1; i >= 0; i--) {
		 if (number[i] == '0') t = 0;
		 if (number[i] == '1') t = 1;
		 if (number[i] == '2') t = 2;
		 if (number[i] == '3') t = 3;
		 if (number[i] == '4') t = 4;
		 if (number[i] == '5') t = 5;
		 if (number[i] == '6') t = 6;
		 if (number[i] == '7') t = 7;
		 if (number[i] == '8') t = 8;
		 if (number[i] == '9') t = 9;
		 res = res + t * d;
		 d = d * 10;
	 }
	 return res;
 }

 bool readMatrix(FILE* fileName, int& n, int& k, int& q) {

	 bool err = false;
	 if (fileName == NULL) {
		 //if there is an error in opening the file - > write error in error.txt
		 FILE* errf = fopen("error.txt", "w");
		 //FILE* errf = NULL;
		 //fopen(&errf, "error.txt", "w");
		 fprintf(errf, "cannot open File!!\n");
		 fclose(errf);
		 err = true;
		 exit(EXIT_FAILURE);
	 }
	 else {
		 unsigned long long one_uul = 1;
		 int count = 0, d;
		 char c = 1;
		 c = getc(fileName);
		 // the file starts with new line
		 // second row: ? k n q current_number_of_matrix
		 // second row is already read in the main function
		 // the elements of the generator matrix are not devided for q<10
		 // the deivider gor q>10 is ','
		 // if any other symbol is entered the function returns error
		 //printf("Reading data form file...\n");
		 while ((c != '\r') && (c != '\n')) {
			 c = getc(fileName);
		 }
		 if (q < 10) {
			 for (int i = 1; i <= k; i++) {	//matrix indexing from [1][0]
				 for (int j = 0; j < n; j++) {
					 c = getc(fileName);
					 if ((c < 48) || (c > 57)) {
						 return true;
					 }

					 if (c == '0')
						 d = 0;
					 if (c == '1')
						 d = 1;
					 if (c == '2')
						 d = 2;
					 if (c == '3')
						 d = 3;
					 if (c == '4')
						 d = 4;
					 if (c == '5')
						 d = 5;
					 if (c == '6')
						 d = 6;
					 if (c == '7')
						 d = 7;
					 if (c == '8')
						 d = 8;
					 if (c == '9')
						 d = 9;
					 matrix.a[i][j] = d;

				 }
				 if (i <= k) {
					 c = 1;
					 int counter = 0;
					 while ((c != '\r') && (c != '\n')) {
						 c = getc(fileName);
						 counter++;
						 if (counter > 2 * n) {
							 return true;
						 }
					 }
				 }
			 }
		 }
		 else {
			 char numberString[2];
			 int stringIt = 0;
			 for (int i = 1; i <= k; i++) { //matrix indexing from [1][0]
				 for (int j = 0; j < n; j++) {
					 c = getc(fileName);
					 if ((c < 48) || (c > 57)) return true;
					 numberString[0] = c;
					 c = getc(fileName);
					 if (c == ',') {
						 numberString[1] = numberString[0];
						 numberString[0] = '0';

					 }
					 else if ((c < 48) || (c > 57)) { return true; }
					 else {
						 numberString[1] = c;
						 c = getc(fileName);
						 //printf("%c\n", c);
					 }

					 matrix.a[i][j] = charStriingToInt(numberString, 2);
					 //cout << matrix[i][j] << "  ";
					 //cout << numberString[0]<<numberString[1] << "  ";
				 }
				 //cout << endl;
				 if (i < k) {
					 c = 1;
					 while ((c != '\r') && (c != '\n')) {
						 c = getc(fileName);
					 }
				 }
			 }
			 c = getc(fileName);
		 }
	 }
	 return err;
 }

 // Prints the global matrix to a file with header and formatting based on q and form.
 void printMatrix(bool form, char* file) {
	 FILE* out = fopen(file, "a");

	 if (out != NULL) {
		 if (form) { fprintf(out, "! "); }
		 else { fprintf(out, "? "); }
		 fprintf(out, "%d %d %d %d\n", matrix.k, matrix.n, matrix.q, matrix.num);
		 if (matrix.q > 9) {
			 for (int i = 0; i < matrix.k; i++) {
				 for (int j = 0; j < matrix.n; j++) {
					 fprintf(out, "%d,", matrix.a[i][j]);
				 }
				 fprintf(out, "\n");
				 //out << endl;
			 }
		 }
		 else {
			 for (int i = 0; i < matrix.k; i++) {
				 for (int j = 0; j < matrix.n; j++) {
					 fprintf(out, "%d", matrix.a[i][j]);
				 }
				 fprintf(out, "\n");
			 }
		 }

		 fprintf(out, "\n");
		 fclose(out);
	 }
	 else {
		 FILE* err = fopen("error.txt", "w");

		 fprintf(err, "Can't open file!!\n");
		 fclose(err);
		 exit(EXIT_FAILURE);
	 }
 }






