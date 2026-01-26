#include"Covering_functions.h"
#include <bitset>
#if defined(_MSC_VER)
#include <intrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#include <cpuid.h>

#endif


static const int N_FIX = 3072; // N_FIX % (512*3) == 0

/// <summary>
/// Constant global variable used for declaration of static arrays, containing different representation of the generator matrix. This parameter referce to the dimension of the code.
/// The maximum possible dimension of the code depends on this value and the number of elements in the finite field.
/// </summary>
/// @note value should be multiple of 12
static const int K_FIX = 36; // K_FIX % (4*3) == 0
static int register_elements = ((N_FIX * 8 - 1) / 128) + 1; // gives the number of registers that will be used
static int Characteristic = 2;

static bool form = false; // if true -> write the codewords as a power of primitive element

/// Transition sequences of Q-ary Grey code for field with characteristics 2
static short int TransitionSequence64[64] = { 0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 6,
                           1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1 };// used to show which copy of the generator matrix is used for calculation of next codeword
/// Transition sequences of Q-ary Grey code for field with characteristics 3
static short int TransitionSequence27[27] = { 0,1,1,2,1,1,2,1,1,3,1,1,2,1,1,2,1,1,3,1,1,2,1,1,2,1,1 };
/// Transition sequences of Q-ary Grey code for field with characteristics 5
static short int TransitionSequence25[25] = { 0,1,1,1,1, 2,1,1,1,1, 2,1,1,1,1, 2,1,1,1,1, 2,1,1,1,1 };
/// Transition sequences of Q-ary Grey code for field with characteristics 7
static short int TransitionSequence49[49] = { 0,1,1,1,1,1,1, 2,1,1,1,1,1,1, 2,1,1,1,1,1,1, 2,1,1,1,1,1,1, 2,1,1,1,1,1,1, 2,1,1,1,1,1,1, 2,1,1,1,1,1,1 };
static __m128i zero; // zero register; used for calculations with byte representation
static __m128i Q_reg_Bytes; // register containing 16 8-byte elemets representing the number of elements in the field; used for calculations with byte representation
int CC[K];
int coef[K], lc[K];
unsigned long long int tover[K][K];



// Constants for static matrices
// Comments are the current values
static const int K_GF2 = K_FIX + 1; //36 + 1
static const int N_GF2 = N_FIX / 8; // 384
static const int N_CH2 = N_FIX; //  3072

static const int K_CH3 = K_FIX + 1; //36 + 1
static const int N_CH3 = N_FIX / 6; // 512

static const int K_P =36*4;// K_FIX / 2 + 1; //18 + 1
static const int N_P =36*4;// N_FIX * 2; // 6144
int POPCNT = 0;

void  add_mod(unsigned char* v1, unsigned char* v2, unsigned  char* result, int q, int n) {
	unsigned long long int w = 0;
	for (int i = 0; i < n; i++) {
		result[i] = (v1[i] + v2[i]) % q;

	}
}

// Function to check if CPU has popcount instruction
void popcnt_detect() {
#if defined(_MSC_VER)
    int abcd[4] = { 0,0,0,0 };
    __cpuid(abcd, 1);
    if (abcd[2] & (1 << 23)) {
        POPCNT = 1;
    }
    __cpuid(abcd, 7);
    if ((abcd[2] & (1 << 14)) && (abcd[1] & (1 << 31))) {
        if (POPCNT == 1) POPCNT = 2;
        else POPCNT = -2;
        return;
    }
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    unsigned int abcd[4] = { 0,0,0,0 };
    __get_cpuid(1, &abcd[0], &abcd[1], &abcd[2], &abcd[3]);
    if (abcd[2] & (1 << 23)) {
        POPCNT = 1;
    }

    __get_cpuid(1, &abcd[0], &abcd[1], &abcd[2], &abcd[3]);
    __get_cpuid_count(7, 0, &abcd[0], &abcd[1], &abcd[2], &abcd[3]);
    if ((abcd[2] & (1 << 14)) && (abcd[1] & (1 << 31))) {
        if (POPCNT == 1) POPCNT = 2;
        else POPCNT = -2;
    }
#endif
}

// function to choose an implementation of popcount based on compiler
// and if there is a popcount CPU instruction
// can be replaced with the specific implementation if the CPU is known
// for faster processing time

long long  popcount(unsigned long long  word) {
    if (POPCNT > 0) {
#if defined(_MSC_VER)
        //return _popcnt64(word); // visual studio with clang
        return _mm_popcnt_u64(word); // visual studio msvc
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__) || defined (__clang__))
        return _popcnt64(word);
#endif
    }
    else {
        unsigned long long t_w;
        unsigned long long w_w;
        t_w = word - ((word >> 1) & 0x5555555555555555L);
        t_w = (t_w & 0x3333333333333333L) + ((t_w >> 2) & 0x3333333333333333L);
        t_w = ((t_w + (t_w >> 4)) & 0x0f0f0f0f0f0f0f0fL);
        w_w = (t_w * 0x0101010101010101L) >> 56;
        return (long long)(w_w);
    }
}

// Static arrays that are used to save the generator matrix into the registers
static union {
  //  __m128i reg128_matrix_GF2[K_GF2][N_GF2 / 2];
  //  unsigned long long int matrix_GF2[K_GF2][N_GF2]; // for GF2 and GF4; bitwise representation of the elements of the field with characteristic 2
  //  __m128i reg128_matrix_CH2[K_GF2][N_CH2 / 16];
  //  unsigned char matrix_CH2[K_GF2][N_CH2]; // for GF8, GF16, GF32, GF64; bytewise representation of the elements ofthe field with characteristic 2
  //  unsigned long long int matrix_CH3[K_CH3][N_CH3]; // for fields with characteristic 3; bitwise representation of the elements
  //  __m128i reg128_matrix_CH3[K_CH3][N_CH3 / 2];
    unsigned char matrix_p[K_P][N_P]; // for ohter finite fields (GF5, GF7, GF11, ..., GF25, ..., GF49, ...); bytewise representation of the elements
    __m128i reg128_matrix_p[K_P][N_P / 16];
};

// Array that save the current linear combination used to calculate next linear combination
static union {
   // __m128i reg128_helper_GF2[K_GF2][N_GF2 / 2];
   // unsigned long long int helper_GF2[K_GF2][N_GF2];
   // unsigned char helper_CH2[K_GF2][N_CH2];
   // __m128i reg128_helper_CH2[K_GF2][N_CH2 / 16];
   // unsigned long long int helper_CH3[K_CH3][N_CH3];
   // __m128i reg128_helper_CH3[K_CH3][N_CH3 / 2];
    unsigned char helper_p[K_P][N_P];
    __m128i reg128_helper_p[K_P][N_P / 16];
};


void setRegistersBytes(dmat_type& bits, int n, int k, int q, int m) {
    // printf( "Set registers\n");
    zero = _mm_setzero_si128();
    Q_reg_Bytes = _mm_set_epi8((char)q, (char)q, (char)q, (char)q, (char)q, (char)q, (char)q, (char)q,
        (char)q, (char)q, (char)q, (char)q, (char)q, (char)q, (char)q, (char)q);

    if (false)//q % 2 == 0)
        {

      /* for (int i = 0; i < K_GF2; i++) {
            for (int j = 0; j < N_CH2; j++) {
                matrix_CH2[i][j] = 0;
                helper_CH2[i][j] = 0;
            }
        }

        for (int i = 1; i <= (m * k); i++) {
            for (int j = 0; j < N; j++) {
                matrix_CH2[i][j] = bits.a[i - 1][j];
            }
        }
        */
    }
    else {
        for (int i = 0; i < K_P; i++) {
            for (int j = 0; j < N_P; j++) {
                matrix_p[i][j] = 0;
                helper_p[i][j] = 0;
            }
        }
        for (int i = 1; i <= matrix.k; i++) {
            for (int j = 0; j < matrix.n; j++) {
                matrix_p[i][j] = matrix.a[i-1 ][j];
               }
        }
    }
}


static inline void add(int rec, int i, int res, int n) {
	__m128i res_add, res_sub;
	for (int col = 0; col < (((n - 1) / 16) + 1); col++) {
		res_add = _mm_add_epi8(reg128_helper_p[rec][col], reg128_matrix_p[i][col]);
		res_sub = _mm_sub_epi8(res_add, Q_reg_Bytes);
		reg128_helper_p[res][col] = _mm_blendv_epi8(res_sub, res_add, res_sub);
	}
}


unsigned long long int k_over_lq(int k, int l, int q) {
	// Number of linear combinations of l rows chosen from a total of k rows, where each row can participate in the linear combination with a coefficient ≤ q.
	// For a field with q elements, each row can participate (q−1) times (with coefficient ≤ (q−1)); therefore, the function is called with (q−1).
	unsigned long long int c = 1;
	//for (int i = k; i >= k - l + 1; i--) {c = c * i;}
	//for (int i = 1; i <= l; i++) {c = c / i;}

	if (l > k) return 0;
	if (l == 0 || l == k) {
		c = 1;
	}
	else {
		int ll = l;
		if (l > k - l) ll = k - l;
		for (unsigned long long int i = 1; i <= ll; i++) {
			c = c * (k - i + 1) / i;
		}
	}
	for (int i = 1; i <= l; i++) {
		c = c * q;
	}
	return c;
}

void maketableover(int k, int l, int q) {
	for (int i = 0; i <= k; i++)
		for (int j = 0; j <= l; j++) {
			tover[j][i] = 0;
		}

	for (int i = 1; i <= k; i++)
		for (int j = 1; j <= l; j++) {
			tover[j][i] = k_over_lq(i, j, q - 1);
		}
}

unsigned long long int sumover(int k, int l) {
	// For a fixed coordinate k of the linear combination,
	// the number of preceding linear combinations equals the number of linear combinations
	// of (l−1) rows from (k−1) + (l−2) rows from (k−1) + ... + 1 row from (k−1).
	// We use l−1 and k−1 because coordinate k is fixed.
	int ll = l;
	if (k < l) {
		ll = k;
	}
	unsigned long long int sumo = 1; // The first linear combination is always 00..010..0
	for (int i = 1; i <= ll; i++) {
		sumo = sumo + tover[i][k];
	}
	return sumo;
}

unsigned long long int pow_ull(unsigned long long int q, unsigned long long int p) {
	unsigned long long int result = 1;
	for (unsigned long long int i = 0; i < p; i++)
		result *= q;
	return result;
}

// Function for computing the binomial coefficient C(n, k)
unsigned long long int binomial(unsigned long long int n, unsigned long long int k) {
	if (k > n) return 0;
	if (k == 0 || k == n) return 1;
	if (k > n - k) k = n - k;  // симетрия
	unsigned long long int result = 1;
	for (unsigned long long int i = 1; i <= k; i++) {
		result = result * (n - i + 1) / i;
	}
	return result;
}

// Volume of a q-ary Hamming ball of radius r
unsigned long long int V_q(unsigned long long int n,
	unsigned long long int r,
	unsigned long long int q)
{
	unsigned long long int sum = 0;
	for (unsigned long long int i = 0; i <= r; i++) {
		unsigned long long int term = binomial(n, i) * pow_ull(q - 1, i);
		sum += term;
	}
	return sum;
}

unsigned long long int HemmingBound(int n, int k, int q) {
	unsigned long long int rhs = pow_ull(q, n - k);
	unsigned long long int r;
	for (r = 0; r <= n; r++) {
		unsigned long long int V = V_q(n, r, q);
		if (V >= rhs) {
			return r;
		}
	}
	// If no r satisfies V >= rhs, return n+1 as an indicator
	return 0;
}


unsigned long long int rank_l_over_k_function(int* CC, int k, int l, int q_local, unsigned long long int A) {
	int i = 1, k_i = k, coef = 0, ll = l, pos_last = 1;
	for (int ii = 1; ii <= k; ii++) { if (CC[ii] != 0)pos_last = ii; } // Find the position of the last nonzero coordinate
	unsigned long long int rank = 0, sum = 0;
	while (CC[i] == 0) { i++; sum = sumover(k_i - 1, l - 1); rank += sum; k_i--; }// Find the number of linear combinations with leading zeros
	// Their number depends on the positions and nonzero coordinates excluding the current fixed coordinate
	rank++; i++; ll--;
	while (i <= pos_last) { // Traverse the linear combination vector until reaching the last nonzero coordinate
		// For the current coordinate, possible values range from 1 to q-1
		// The first linear combination is always x1,..,x_l,1,00..0, where x1 to x_l have already been processed
		if (CC[i] == 0) {
			coef = q_local - 1;
			sum = sumover(k - i, ll - 1);
		}
		else {
			coef = CC[i] - 1;
			sum = sumover(k - i, ll - 1);
			ll--; // If we add an element to the linear combination at the current coordinate, we decrease the number of elements that can still be added
			rank = rank + 1; // The first vector needs to be added
		}
		rank = rank + coef * sum;
		i++;
	}
	if (rank != A) {
		printf("!!error rank\n");
	}
	return rank;
}

// Generate the vector representing the linear combination for the given rank
void unrankBBgen(unsigned long long int A, int q, int k, int l) {

	for (int i = 0; i < K; i++) {
		CC[i] = 0;
	}
	unsigned long long int A_beg = A, rank = 0;
	int kh = k, kb = 1, ll = l;
	bool ok = true;
	unsigned long long int sum = 0;
	// Find the first nonzero coordinate: it must be 1;
	// Decrease A by (k-1 over l-1) * q^(l-1) for (l-1), ..., (1), then assign 1
	while (ok) {
		sum = sumover(kh - 1, l - 1);
		if (A > sum)
		{
			A = A - sum; //sumover(kh - 1, l - 1);
			kh--;
			kb++;
		}
		else {
			ok = false;
			CC[kb] = 1;
			A--;
			if (0 == A) {
				return;
			}
			kh--;
			kb++;
		}
	}
	ll--;	// The first coordinate is set, and the linear combination is decreased by one

	while (kb <= k) {
		int step = 1;
		while (step <= q) {
			if (step < q) {
				sum = sumover(k - kb, ll - 1);
				if (A > sum) {
					A = A - sum;	// (sumover(k - kb, ll - 1));
					step++;
				}
				else {
					CC[kb] = step % q;
					if (step != q) {
						A--;
						ll--;
						if (0 == A) {
							rank = rank_l_over_k_function(CC, k, l, q, A_beg);
							return;
						}
					}
					kb++;
					step = 1;
				}
			}
			else {
				CC[kb] = 0;
				kb++;
				break;
			}
		}
	}
}

