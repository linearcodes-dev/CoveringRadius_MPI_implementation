#include <iostream>
#include"Polynomials.h"
#include"Data.h"

const int K = 100, N = 100, Q = 2;


bool is_prime_power[65] = { true, true, true, true, true, true, false, true, true, true,
false, true, false, true, false, false, true, true, false, true, false, false, false,
true, false, true, false, true, false, true, false, true, true, false, false, false,
false, true, false, false, false, true, false, true, false, false, false, true, false, true,
false, false, false, true, false, false, false, false, false, true, false, true, false, false, true };
/*
bool init_zero(int covering) {
	//printf(" INIT ZEROn = %d k = %d q = %d\n\n", n, k, q);
	//return 1;

	bool enough_mem = true;

	if (enough_mem) {
		dmat_short_new(matrix, K + 1, N , Q);
		dmat_short_new(matrixH, K + 1, N , Q);

		// same as in lib128 !!!!!!!!!!!!!!! //
		if (covering == 1) {

			dmat_new(bitsCharCF, K, N, Q);
			for (int i = 0; i < bitsCharCF.k + 1; i++) {
				for (int j = 0; j < bitsCharCF.n; j++) {
					bitsCharCF.a[i][j] = 0;
				}
			}
		}
		else {
			return false;
		}


		for (int i = 0; i < matrix.k; i++) {
			for (int j = 0; j < matrix.n; j++) {
				matrix.a[i][j] = 0;
				matrixH.a[i][j] = 0;
			}
		}
		if (bits.mem != 0) {
			for (int i = 0; i < bits.k; i++) {
				for (int j = 0; j < bits.n; j++) {
					bits.a[i][j] = 0;
				}
			}
		}
		if (bitsCharCF.mem != 0) {
			for (int i = 0; i < bitsCharCF.k; i++) {
				for (int j = 0; j < bitsCharCF.n; j++) {
					bitsCharCF.a[i][j] = 0;
				}
			}
		}
		return true;
	}

	else {
		return false;
	}
}

bool init_covering() {
	//maketable(Q);
	num_to_coef_char2(K, N, 1);
	return true;
}

*/


unsigned long long int QN[50];
// 2^21

static unsigned long long int power(int q, int k) {
	unsigned long long int p = 1; QN[0] = 1;
	for (int i = 1; i <= 49; i++) {
		p = p * q;
		QN[i] = p;
	}
	return p;
}


unsigned long long pointtointi(char* b, int len)
{
	int i, j;
	unsigned long long a1 = 0, a = 0;
	i = len - 1;
	while (0 == b[i])
	{
		i--;
		if (0 > i) { printf("ERROR point"); }
	}


	if (b[i] != 1) {
		printf("ERROR !=1\n");
	}
	
	for (j = i - 1; j >= 0; j--) // When it is -2 => -1 because indexing starts from the first position; -1 because we start counting from coordinate 1
	{
		a = a + QN[j];
	}

	for (j = i - 1; j >= 0; j--) // -1 because we start counting from coordinate 1
	{
		a1 = a1 + b[j];
		if (j > 0) { a1 = a1 * QN[1]; }
	}

	a = a + a1 + 1;

	return a;
}

// Convert finite-field vector 'b' to a unique integer; 0 if all zeros
unsigned long long int pointtointi_lr(unsigned char* b, int len)
{
	int i, j;
	short temp;
	unsigned long long int a1 = 0, a = 0;
	unsigned char local[100];
	// Copy input vector into a local array
	for (i = 0; i < len; i++) {
		local[i] = b[i];
	}

	// Skip leading zeros
	i = 0;
	while (0 == local[i])
	{
		i++;
		if (i >= len) {
			return 0; // Return 0 if vector is all zeros
		}
	}
	// Normalize first nonzero element to 1
	if (local[i] != 1) {
		temp = inverse(local[i]);
		for (j = i; j < len; j++)
			local[j] = multiply(local[j], temp);
	}
	// Compute contribution of QN coefficients for positions after first nonzero
	for (j = i + 1; j < len; j++) 
	{
		a = a + QN[len - 1 - j];
	}
	// Compute contribution of remaining vector elements
	for (j = i + 1; j < len; j++) 
	{
		a1 = a1 + local[j];
		if (j < (len - 1)) { a1 = a1 * QN[1]; }
	}
	// Combine contributions and add 1
	a = a + a1 + 1;
	return a;
}

//// Convert an integer 'a' into a finite-field vector 'b' of length 'len'
//void inttopointic(int a, short len, char* b)
//{
//	int i;
//	for (i = 0; i <= len; i++)
//		b[i] = 0;
//	i = len - 1; 
//	while (a > QN[len - 1 - i]) 
//	{
//		a = a - QN[len - 1 - i];;
//		i--; 
//	}
//	a = a - 1;
//	b[i] = 1;
//	i = len - 1; 
//	while (a > 0)
//	{
//		b[i] = a % QN[1];
//		a = a / QN[1];
//		i--; 
//	}
//}


