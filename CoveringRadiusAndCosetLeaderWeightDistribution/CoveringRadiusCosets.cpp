
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>
//#include"omppar.h"


#if defined(_MSC_VER)
#include <intrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#include <cpuid.h>

#endif

using namespace std;


unsigned long long  nuh[32][32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
int nth[16]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0 };
int sum = 0;
struct  partt {
    unsigned char a[128];
}part;


const unsigned long long int Maxn = 64, MaxProc = 16, MaxCW = 100000, MAX = 1000; //25937424601; //28629151*31*31; //2147483647/2;//33554432;
int g[Maxn];
//int rank, size;

//int T[MaxCW];//, T_single[MaxCW];
unsigned long long int NN = 0;
clock_t start_t, end_t;
int result = 0;

unsigned long long counter[MaxProc];
int SIZE = 0, RANK = 0;

#ifndef ERRORQ
#define ERRORQ(expr) \
        if ((expr)) { \
		FILE *fran;\
	    fran=fopen("ERRORq.txt", "w");\
		fprintf(fran,"qext file %s: line %d: assertion failed: " \
			"(%s)\n",__FILE__,__LINE__,#expr); \
			int i=fclose(fran); \
			int t;\
           printf("ERROR");\
           scanf("%d", &t);\
	while(true)\
	{\
		printf("ERROR");\
        	        }\
			exit(1);\
	}
#endif /* !ASSERT */

int charStriingToInt(char* number, int len) {
    // for reading decimal numbers >10
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

long long  popcount(unsigned long long  word) {
    //if (POPCNT>0) {
#if defined(_MSC_VER)
        //return _popcnt64(word); // visual studio with clang
    return _mm_popcnt_u64(word); // visual studio msvc
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__) || defined (__clang__))
    return _popcnt64(word);
#endif
    /*}
    else {
        unsigned long long t_w;
        unsigned long long w_w;
        t_w = word - ((word >> 1) & 0x5555555555555555L);
        t_w = (t_w & 0x3333333333333333L) + ((t_w >> 2) & 0x3333333333333333L);
        t_w = ((t_w + (t_w >> 4)) & 0x0f0f0f0f0f0f0f0fL);
        w_w = (t_w * 0x0101010101010101L) >> 56;
        return (long long)(w_w);
    }*/
}


//================================================================

void print(int g[], int n, int t) {
    for (int j = 1; j <= n; j++) { printf("%d", g[j]); }
    printf(" T= %d; \n", t);
}

//==========================================print parallel===========================//

void print(int g[], int n, int t, int rank, bool task) {
    char fname[300];
    FILE* fr;
    if (task) sprintf(fname, "Gray_Task_%d.txt", rank); //itoa(rank, fname,10);
    else sprintf(fname, "Gray_Rank_%d.txt", rank); //itoa(rank, fname,10);
    //strcat(fname, ".yyy");
    fr = fopen(fname, "a");
    for (int j = 1; j <= n; j++) {
        fprintf(fr, "%d,", g[j]);
        //printf("%d", g[j]);
    } //printf("\n");
    fprintf(fr, "\n");
    //fprintf(fr," T= %d; N=%d\n", t, NN);
    fclose(fr);
}

void print_rec(int g[], int n, int t) {
    char fname[300];
    FILE* fr;
    sprintf(fname, "Gray_Task.txt"); //itoa(rank, fname,10);
    //strcat(fname, ".yyy");
    fr = fopen(fname, "a");
    for (int j = 1; j <= n; j++) {
        fprintf(fr, "%d,", g[j]);
        //printf("%d", g[j]);
    } //printf("\n");
    fprintf(fr, "\n");
    //fprintf(fr," T= %d; N=%d\n", t, NN);
    fclose(fr);
}






FILE* fname;
unsigned long long int total_omp = 0;
unsigned long long int total_rec = 0;
int R = 0;
unsigned long long int D = Maxn;
unsigned long long int weights[4][Maxn + 1];
char matrix[Maxn + 1][Maxn];
char matrixH[Maxn + 1][Maxn];
int N, K, Q;


bool readMatrix() {
    bool err = false;
    string in = "";
    char* fileName;
    // cin >> in;
    fileName = new char[in.length() + 1];
    strcpy(fileName, in.c_str());
    FILE* input = fopen("EXAM", "r");
    if (input == NULL) {
        //if there is an error in opening the file - > write error in error.txt
        FILE* errf = fopen("error.txt", "w");
        fprintf(errf, "cannot open File!!\n");
        fclose(errf);
        err = true;
        exit(EXIT_FAILURE);
    }
    else {
        unsigned long long one_uul = 1;
        int count = 0, d;
        char c = 1;
        c = getc(input); c = getc(input);
        if (c == '?' || c == '!') {
            fscanf(input, "%d", &K);
            fscanf(input, "%d", &N);
            fscanf(input, "%d", &Q);
            fscanf(input, "%d", &c);
        }

        c = getc(input);
        // the file starts with new line
        // second row: ? k n q current_number_of_matrix
        // second row is already read in the main function
        // the elements of the generator matrix are not devided for q<10
        // the deivider gor q>10 is ','
        // if any other symbol is entered the function returns error
        //printf("Reading data form file...\n");
        while ((c != '\r') && (c != '\n')) {
            c = getc(input);
        }
        if (Q < 10) {
            for (int i = 1; i <= K; i++) {	//matrix indexing from [1][0]
                for (int j = 0; j < N; j++) {
                    c = getc(input);
                    if ((c < 48) || (c > 57)) return true;
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
                    matrix[i][j] = d;

                }
                if (i <= K) {
                    c = 1;
                    int counter = 0;
                    while ((c != '\r') && (c != '\n')) {
                        c = getc(input);
                        counter++;
                        if (counter > 2 * N) {
                            return true;
                        }
                    }

                }

            }
        }
        else {
            char numberString[2];
            int stringIt = 0;
            for (int i = 1; i <= K; i++) { //matrix indexing from [1][0]
                for (int j = 0; j < N; j++) {
                    c = getc(input);
                    if ((c < 48) || (c > 57)) return true;
                    numberString[0] = c;
                    c = getc(input);
                    if (c == ',') {
                        numberString[1] = numberString[0];
                        numberString[0] = '0';

                    }
                    else if ((c < 48) || (c > 57)) { return true; }
                    else {
                        numberString[1] = c;
                        c = getc(input);
                        //printf("%c\n", c);
                    }

                    matrix[i][j] = charStriingToInt(numberString, 2);
                    //cout << matrix[i][j] << "  ";
                    //cout << numberString[0]<<numberString[1] << "  ";
                }
                //cout << endl;
                if (i < K) {
                    c = 1;
                    while ((c != '\r') && (c != '\n')) {
                        c = getc(input);
                    }

                }
            }
            c = getc(input);
        }

    }

    return err;
}



void dreadgmatvcodeblock() { 

	FILE* fpr = fopen("EXAM", "r");

	char genmatname[50];
	int d = 0, m = 0, n = 0, q = 0;

	char* ch;
	char cc, ccd = 1;
	char chh[301];

	while (!(feof(fpr))) {
		ccd = getc(fpr);
		if (ccd == '?') {
			fscanf(fpr, "%d", &m);
			fscanf(fpr, "%d", &n);

			fscanf(fpr, "%d", &q);
			K = m;
			N = n;
			Q = q;
			//printf("OK12  %d ,%d %d \n",K,N,Q);
			/*
			fscanf(fpr, "%s", &chh);
			*/

			fgets(chh, 300, fpr);
			//printf(" S %s", chh);
			int y = 0;
			if (chh[0] != '\n') {
				strcpy(genmatname, "");
				int ii = 0;
				while (chh[ii] != '\n') {
					genmatname[ii] = chh[ii];
					ii++;
				}
				genmatname[ii] = '\0';
				//strcpy(genmat.name, chh);
			}
			else {
				strcpy(genmatname, "  ");
			}

			//printf("NAME %s", genmatname);
			// int j=sizeof( char);
			ch = (char*)malloc((n) * 4);
			int lch = n * 4;
			//fgets(ch, sizeof(ch), fpr);
		   // dmat_new(genmat,m,n,q); //otdelia pamet za matricata ako ne e otdelena

		   // maketable(genmat.q); // zaregda tablicite na poleto

			//genmat.k=genmat.m;
			char c = 1;
			// while (c!='\n'){
			//	 c=getc(fpr);}
			for (int kk = 1; kk <= m; kk++) {
				fgets(ch, lch, fpr);
				int len = 0;
				while (ch[len])
				{
					len++;
				}
				//int y=sizeof(ch*);
				if ('\n' == ch[len - 1]) {
					len--;
				}
				int com = 0;
				for (int i = 0; i < len; i++) {
					if (',' == ch[i])
					{
						com++;
					}
					// if('\n'==ch[i]){len=i-1;break;}
				}

				char ct, st[10];
				int tt = -1, nn = 0, comh = 0;
				if ((com > 0) && (com != n)) {
					ERRORQ((com > 0) && (com != n));
				}
				if (com > 0) { // ako ima zapetajki
					for (int i = 0; i < len; i++) {
						if (',' != ch[i]) {
							if ((48 <= ch[i]) && (48 + q - 1 >= ch[i])) {
								tt++; st[tt] = ch[i];
							}
							else {
								ERRORQ(!((48 <= ch[i]) && (48 + q - 1 >= ch[i])))
							}
						}
						if (',' == ch[i])
						{
							tt++;
							st[tt] = '\n';
							int r = atoi(st);
							nn++;
							matrix[kk][nn-1] = atoi(st);;
							tt = -1;
							comh++;
							if (comh == n) {
								i = len;
								break;
							}
						}

					}
				}
				else {// ako niama zapetajki megdu elementite - bsiako chislo e element
					for (int i = 0; i < n; i++) {
						if ((48 <= ch[i]) && (48 + q - 1 >= ch[i])) {
						}
						else {
							ERRORQ(!((48 <= ch[i]) && (48 + q - 1 >= ch[i])))
						}

						st[0] = ch[i];
						st[1] = '\n';
						int r = atoi(st);
						nn++;
						matrix[kk][nn-1] = atoi(st);;
					}
				}// kraj ako niama zapetajki
			}
			free(ch);
			for (int i = 1; i < K; i++) {
				//printf("\n");
				for (int j = 1; j < N; j++) {
				//	printf("%d,", matrix[i][j]);
				}
			}
			return;
		}
	}

}



void randomgenf(int n, int k, int q, char matrix[Maxn + 1][Maxn])
{

    /* Intializes random number generator */
    time_t t;

    /* Intializes random number generator */
    srand((unsigned)time(&t));

    FILE* fran;
    //q=5;
    fran = fopen("EXAM", "w");
    // Print n random numbers.
    int ki, ni;
    fprintf(fran, "\n");
    //for (numi = 1; numi <= num; numi++)
    //{

    { fprintf(fran, "? %d %d %d %d \n", k, n, q, 1); }
    for (ki = 1; ki <= k; ki++) // from 1 to k+1
    {
        for (ni = 0; ni < n; ni++) {
            if (ni + 1 > k) {
                int qh;
                if ((ni + 1) == (k + 1)) { qh = 1; }
                else { qh = rand() % q; }
                if (q > 10) {
                    fprintf(fran, "%d,", qh);
                    matrix[ki][ni] = qh;
                }
                else {
                    fprintf(fran, "%d", qh);
                    matrix[ki][ni] = qh;
                }
            }
            else {
                if (ni + 1 == ki) {
                    if (q > 10) {
                        fprintf(fran, "1,");
                        matrix[ki][ni] = 1;
                    }
                    else {
                        fprintf(fran, "1");
                        matrix[ki][ni] = 1;
                    }
                }
                else {
                    if (q > 10) {
                        fprintf(fran, "0,");
                        matrix[ki][ni] = 0;
                    }
                    else {
                        fprintf(fran, "0");
                        matrix[ki][ni] = 0;
                    }
                }
            }
        }
        fprintf(fran, "\n");
    }
    //}
    int i = fclose(fran);
};


void printMat(){
    printf("N = %d, K = %d Q = %d\n", N,K,Q);
for(int i = 1; i<=K; i++){
    for (int j = 0; j<N; j++){
        printf("%d",(int)matrix[i][j]);
    }
    printf("\n");
}
}

struct lc {
    char vec[Maxn];
    //int elements;
};
struct vecULL {
    unsigned long long int vec[Maxn + 1];
};

struct mat {
    // char matrix[(Maxn + 1)][Maxn];
    char matrix[(Maxn + 1) * Maxn];
};
struct matULL {
    unsigned long long int matrix[16][Maxn + 1];
}weightsGlobal;


mat helper;

void mat_init(mat& h) {
    for (int i = 0; i <= Maxn; i++) {
        for (int j = 0; j < Maxn; j++) {
            h.matrix[i * Maxn + j] = 0;
        }
    }
}

void matULL_init(matULL& h) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j <= Maxn; j++) {
            h.matrix[i][j] = 0;
        }
    }
}
void vec_init(vecULL& vec) {
    for (int i = 0; i <= Maxn; i++) {
        vec.vec[i] = 0;
    }
}
void lc_init(lc& vec) {
    for (int i = 0; i <= Maxn; i++) {
        vec.vec[i] = 0;
    }
}

void lc_printf(lc& vec, int n, int th_id) {

    char fname[300];
    FILE* fr;
    sprintf(fname, "TH_%d.txt", th_id); //itoa(rank, fname,10);
    //strcat(fname, ".yyy");
    fr = fopen(fname, "a");


    for (int i = 0; i < n; i++) {
        fprintf(fr, "%d", (int)vec.vec[i]);
    }
    fprintf(fr, "\n");
    //fprintf(fr,"    %d\n", vec.elements);

    fclose(fr);
}

void lc_print(lc& vec, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d", (int)vec.vec[i]);
    }
    // printf( "    %d\n", vec.elements);
    printf("\n");
}

void print_matrix(mat& helper, int row, int n, int th_id) {
    char fname[300];
    FILE* fr;
    sprintf(fname, "shared_mat_TH_%d.txt", th_id); //itoa(rank, fname,10);
    //strcat(fname, ".yyy");
    fr = fopen(fname, "a");
    for (int ii = 0; ii < n; ii++) {
        // matrixH[rec][ii] = (matrix[i][ii] + matrixH[rec][ii]) % q;
         //total_rec ++;
        fprintf(fr, "%d", (int)helper.matrix[row * Maxn + ii]);
    }
    fprintf(fr, "\n");
    fclose(fr);
}

static inline unsigned long long int add_weight(lc& a, char* b, lc& c, int Q, int N) {

    __m128i Q_reg_Bytes = _mm_set_epi8((char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q,
        (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q);
    __m128i zero = _mm_setzero_si128();
    __m128i res_add, res_sub, v1, v2, v3; //v3 = v1 + v2;
    __m128i cmpl;
    __m128i h = _mm_set_epi8(2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1);

    unsigned long long int w = 0;
    for (int col = 0; col < N; col += 16) {
        v1 = _mm_loadu_si128((__m128i*) & a.vec[col]);
        //v2 = _mm_set_epi8(b[col],b[col+1],b[col+2],b[col+3],b[col+4],b[col+5],b[col+6],b[col+7],b[col+8],
        //                  b[col+9],b[col+10],b[col+11],b[col+12],b[col+13],b[col+14],b[col+15]);

        v2 = _mm_loadu_si128((__m128i*) & b[col]);

        res_add = _mm_add_epi8(v1, v2);
        res_sub = _mm_sub_epi8(res_add, Q_reg_Bytes);
        v3 = _mm_blendv_epi8(res_sub, res_add, res_sub);
        _mm_storeu_si128((__m128i*) & c.vec[col], v3);
        //(c.vec[col]) = (char*)&v3;

        cmpl = _mm_cmpeq_epi8(zero, v3);
        v1 = _mm_and_si128(cmpl, h); //r2
        v2 = _mm_srli_si128(v1, 8); // r3
        v3 = _mm_or_si128(v1, v2); //r4

        unsigned long long* t = (unsigned long long*) & v3;
        w = w + (16 - popcount(t[0]));
    }
    return w;

}


static inline unsigned long long int add_weight_mat(char* a, char* b, char* c, int q, int n) {
    //printf("Q = %d N=%d\n", Q, N);
    __m128i Q_reg_Bytes = _mm_set_epi8((char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q,
        (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q, (char)Q);
    __m128i zero = _mm_setzero_si128();
    __m128i res_add, res_sub, v1, v2, v3; //v3 = v1 + v2;
    __m128i cmpl;
    __m128i h = _mm_set_epi8(2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1);

    unsigned long long int w = 0;
    for (int col = 0; col < N; col += 16) {
        v1 = _mm_loadu_si128((__m128i*) & a[col]);
        //v2 = _mm_set_epi8(b[col],b[col+1],b[col+2],b[col+3],b[col+4],b[col+5],b[col+6],b[col+7],b[col+8],
        //                  b[col+9],b[col+10],b[col+11],b[col+12],b[col+13],b[col+14],b[col+15]);

        v2 = _mm_loadu_si128((__m128i*) & b[col]);

        res_add = _mm_add_epi8(v1, v2);
        res_sub = _mm_sub_epi8(res_add, Q_reg_Bytes);
        v3 = _mm_blendv_epi8(res_sub, res_add, res_sub);
        _mm_storeu_si128((__m128i*) & c[col], v3);
        //(c.vec[col]) = (char*)&v3;

        cmpl = _mm_cmpeq_epi8(zero, v3);
        v1 = _mm_and_si128(cmpl, h); //r2
        v2 = _mm_srli_si128(v1, 8); // r3
        v3 = _mm_or_si128(v1, v2); //r4

        unsigned long long* t = (unsigned long long*) & v3;
        w = w + (16 - popcount(t[0]));
    }
    return w;

}


unsigned long long int  add_mod(lc& v1, char* v2, lc& result, int q, int n) {
    unsigned long long int w = 0;
    // printf("lc1 elemets = %d \n", v1.elements);
    for (int i = 0; i < n; i++) {
        result.vec[i] = (v1.vec[i] + v2[i]) % q;
        if (result.vec[i] > 0) w++;
    }
    // result.elements = v1.elements + 1;

}

unsigned long long int  add_weight_mod(char* v1, char* v2, char* result, int q, int n) {
    unsigned long long int w = 0;
    for (int i = 0; i < n; i++) {
        result[i] = (v1[i] + v2[i]) % q;
        if (result[i] > 0) w++;
    }
    // result.elements = v1.elements + 1;
    return w;
}

//vecULL wLocal;
int taskNum = 0;


unsigned long long int d_local = Maxn;

unsigned long long int coset_leader_WD[100];
bool end_MPI = false;
void linear_combinations_min_dis(int rec, int h, int n, int k, int q, mat& helper, unsigned long long int& d) {// // min dis of coset SSE only
    int qf = q;

    {
        for (int i = h; i <= k; i++) {
            for (int q1 = 1; q1 < qf; q1++) {
                //if (end_MPI) return;
                if (q1 == 1) {
                    unsigned long long int w = add_weight_mat(helper.matrix + ((rec - 1) * Maxn), matrix[i], helper.matrix + ((rec)*Maxn), q, n);
                    if (w < d) d = w;

                }
                else {
                    unsigned long long int w = add_weight_mat(helper.matrix + ((rec)*Maxn), matrix[i], helper.matrix + ((rec)*Maxn), q, n);
                    if (w < d) d = w;
                }
                // if (D < R) end_MPI = true;
                if (rec < k) {

                    linear_combinations_min_dis(rec + 1, i + 1, n, k, q, helper, d);

                }
            }
        }


    }


}
void linear_combinations_min_dis_CR(int rec, int h, int n, int k, int q, mat& helper) {//// min dis of coset for calculation of R without coset leader distribution
    int qf = q;

    {
        for (int i = h; i <= k; i++) {
            for (int q1 = 1; q1 < qf; q1++) {
                if (end_MPI) return;
                if (q1 == 1) {
                    unsigned long long int w = add_weight_mat(helper.matrix + ((rec - 1) * Maxn), matrix[i], helper.matrix + ((rec)*Maxn), q, n);
                    if (w < D) D = w;


                }
                else {
                    unsigned long long int w = add_weight_mat(helper.matrix + ((rec)*Maxn), matrix[i], helper.matrix + ((rec)*Maxn), q, n);
                    if (w < D) D = w;

                }

                if (D < R) end_MPI = true;
                if (rec < k) {

                    linear_combinations_min_dis_CR(rec + 1, i + 1, n, k, q, helper);

                }
            }
        }


    }


}

void Gray_modular_nonproportional_by_stack(int m, int n, int k, int& br, mat& helper, int np) { //generating coset representatives

    // np=0 - all codewords; np=1 - only nonproportional codewords
    int T[Maxn], M[Maxn];
    int i, j, t;
    for (int j = 0; j <= n; j++) { // initialization
        T[j] = j + 1;
        if (1 == np) M[j] = 1;
        else M[j] = m - 1;
        g[j] = 0;
    }
    i = 1;

    do {
        j = n + 1 - i;
        g[j] = (g[j] + 1) % m;
        t = j;
        br++;

            // print(g, n, t);
            for (int ii = k; ii < n + k; ii++) {
                helper.matrix[ii] = g[ii - k + 1];
            }

            D = N;
            unsigned long long int w = add_weight_mat(helper.matrix, matrix[0], helper.matrix, Q, N);
            if (w < D) D = w;
            // weights[0][w]++;
            linear_combinations_min_dis(1, 1, n + k, k, m, helper, D);
            coset_leader_WD[D]++;
            if (D > R) R = D;



        M[i] = M[i] - 1;
        T[0] = 1;
        if (0 == M[i]) {
            T[i - 1] = T[i];
            T[i] = i + 1;
            M[i] = m - 1;
        };
        i = T[0];
    } while (i != n + 1);
}


void Gray_modular_nonproportional_by_stack_MPI(int m, int n, int k, int& br, mat& helper, int np) { //generating coset representatives MPI

        MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
        MPI_Comm_size(MPI_COMM_WORLD, &SIZE);

        // np=0 - all codewords; np=1 - only nonproportional codewords
    int T[Maxn], M[Maxn];
    int i, j, t;
    for (int j = 0; j <= n; j++) { // initialization
        T[j] = j + 1;
        if (1 == np) M[j] = 1;
        else M[j] = m - 1;
        g[j] = 0;
    }
    i = 1;

    do {
        j = n + 1 - i;
        g[j] = (g[j] + 1) % m;
        t = j;
        br++;
        if (RANK == br % SIZE) {
            // print(g, n, t);
            for (int ii = k; ii < n + k; ii++) {
                helper.matrix[ii] = g[ii - k + 1];
            }

            D = N; end_MPI = false;
            unsigned long long int w = add_weight_mat(helper.matrix, matrix[0], helper.matrix, Q, N);
            if (w < D) D = w;
            // weights[0][w]++;
            linear_combinations_min_dis(1, 1, n + k, k, m, helper, D);
            coset_leader_WD[D]++;
            if (D > R) R = D;
        }


        M[i] = M[i] - 1;
        T[0] = 1;
        if (0 == M[i]) {
            T[i - 1] = T[i];
            T[i] = i + 1;
            M[i] = m - 1;
        };
        i = T[0];
    } while (i != n + 1);
}

void Gray_modular_nonproportional_by_stack_omp_cosets(int m, int n, int k, int& br, mat& helper, int np) { //generating coset representatives OMP; paralleization by cosets only
   // MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    //MPI_Comm_size(MPI_COMM_WORLD, &SIZE);

    // np=0 - all codewords; np=1 - only nonproportional codewords
    int T[Maxn], M[Maxn];
    int i, j, t;
    for (int j = 0; j <= n; j++) { // initialization
        T[j] = j + 1;
        if (1 == np) M[j] = 1;
        else M[j] = m - 1;
        g[j] = 0;
    }
    i = 1;

    do {
        j = n + 1 - i;
        g[j] = (g[j] + 1) % m;
        t = j;
        br++;
        //if (RANK == br % SIZE) {
            // print(g, n, t);
            for (int ii = k; ii < n + k; ii++) {
                helper.matrix[ii] = g[ii - k + 1];
            }

            D = N;
            unsigned long long int w = add_weight_mat(helper.matrix, matrix[0], helper.matrix, Q, N);
            if (w < D) D = w;
            //weights[0][w]++;

#pragma omp task firstprivate (D)//helper
            {
                mat helper_local;
                for (int ii = 0; ii < (Maxn + 1) * Maxn; ii++) {
                    helper_local.matrix[ii] = helper.matrix[ii];
                }
                linear_combinations_min_dis(1, 1, n + k, k, m, helper_local, D);
#pragma omp critical
                {
                    coset_leader_WD[D]++;
                    if (D > R) R = D;
                }
            }


        //}


        M[i] = M[i] - 1;
        T[0] = 1;
        if (0 == M[i]) {
            T[i - 1] = T[i];
            T[i] = i + 1;
            M[i] = m - 1;
        };
        i = T[0];
    } while (i != n + 1);
}



void Gray_modular_nonproportional_by_stack_MPI_OpenMP_cosets(int m, int n, int k, int& br, mat& helper, int np) { //generating coset representatives MPI+OMP; paralleization by cosets only
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);

    // np=0 - all codewords; np=1 - only nonproportional codewords
    int T[Maxn], M[Maxn];
    int i, j, t;
    for (int j = 0; j <= n; j++) { // initialization
        T[j] = j + 1;
        if (1 == np) M[j] = 1;
        else M[j] = m - 1;
        g[j] = 0;
    }
    i = 1;

    do {
        j = n + 1 - i;
        g[j] = (g[j] + 1) % m;
        t = j;
        br++;
        if (RANK == br % SIZE) {
            // print(g, n, t);
        for (int ii = k; ii < n + k; ii++) {
            helper.matrix[ii] = g[ii - k + 1];
        }

        D = N;
        unsigned long long int w = add_weight_mat(helper.matrix, matrix[0], helper.matrix, Q, N);
        if (w < D) D = w;
        //weights[0][w]++;

#pragma omp task firstprivate (D) //helper
        {
            mat helper_local;
            for (int ii = 0; ii < (Maxn + 1) * Maxn; ii++) {
                helper_local.matrix[ii] = helper.matrix[ii];
            }
            linear_combinations_min_dis(1, 1, n + k, k, m, helper_local, D);
#pragma omp critical
            {
                coset_leader_WD[D]++;
                if (D > R) R = D;
            }
        }


        }


        M[i] = M[i] - 1;
        T[0] = 1;
        if (0 == M[i]) {
            T[i - 1] = T[i];
            T[i] = i + 1;
            M[i] = m - 1;
        };
        i = T[0];
    } while (i != n + 1);
}




void covering_rad_beg_MPI() { // begin MPI
    int br = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
   // dreadgmatvcodeblock();
    //  randomgenf(n, k, q, matrix);
   // readMatrix();
    for (int i = 0; i < 100; i++) {
        coset_leader_WD[i] = 0;
    }
    for (int i = 0; i <= Maxn; i++) {
        weights[0][i] = 0;
    }

    mat_init(helper);
    R = 0;
    Gray_modular_nonproportional_by_stack_MPI(Q, N - K, K, br, helper, 1);
    // printf("rank = %d R=%d\n\n", RANK, R);


    int R_global = 0;
    unsigned long long int coset_leader_WD_reduce[100];
    for (int i = 0; i < 100; i++) {
        coset_leader_WD_reduce[i] = 0;
    }
    MPI_Reduce(&R, &R_global, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(coset_leader_WD, coset_leader_WD_reduce, 100, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (RANK == 0) {
        printf("Global covering rad = : %d\n", R_global);
        for (int i = 0; i < 100; i++) {
            if (coset_leader_WD_reduce[i] > 0)
                printf("%d^%d   ", i, coset_leader_WD_reduce[i] * (Q - 1));
        }

    }

}


void covering_rad_beg_SSE_only () { // begin SSE
    int br = 0;

   // dreadgmatvcodeblock();
    //  randomgenf(n, k, q, matrix);
    //readMatrix();
    for (int i = 0; i < 100; i++) {
        coset_leader_WD[i] = 0;
    }
    for (int i = 0; i <= Maxn; i++) {
        weights[0][i] = 0;
    }

    mat_init(helper);
    R = 0;
    Gray_modular_nonproportional_by_stack(Q, N - K, K, br, helper, 1);

        printf("Global covering rad = : %d\n", R);
        for (int i = 0; i < 100; i++) {
            if (coset_leader_WD[i] > 0)
                printf("%d^%d   ", i, coset_leader_WD[i] * (Q - 1));
        }



}


void covering_rad_beg_MPI_OMP_cosets() { // begin MPI+OMP cosets
    int br = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
    //  randomgenf(n, k, q, matrix);
   // dreadgmatvcodeblock();
    //readMatrix();
    for (int i = 0; i < 100; i++) {
        coset_leader_WD[i] = 0;
    }
    for (int i = 0; i <= Maxn; i++) {
        weights[0][i] = 0;
    }

    mat_init(helper);
    R = 0;
    //omp_set_num_threads(4);
#pragma omp parallel
    {
#pragma omp single nowait
        Gray_modular_nonproportional_by_stack_MPI_OpenMP_cosets(Q, N - K, K, br, helper, 1);
    }

    int R_global = 0;
    unsigned long long int coset_leader_WD_reduce[100];
    for (int i = 0; i < 100; i++) {
        coset_leader_WD_reduce[i] = 0;
    }
    MPI_Reduce(&R, &R_global, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(coset_leader_WD, coset_leader_WD_reduce, 100, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (RANK == 0) {
        printf("Global covering rad = : %d\n", R_global);
        for (int i = 0; i < 100; i++) {
            if (coset_leader_WD_reduce[i] > 0)
                printf("%d^%d   ", i, coset_leader_WD_reduce[i] * (Q - 1));
        }

    }

}





void covering_rad_beg_OpenMP_cosets() {
    int br = 0;
   
    //  randomgenf(n, k, q, matrix);
   // readMatrix();
    for (int i = 0; i < 100; i++) {
        coset_leader_WD[i] = 0;
    }


    mat_init(helper);
    R = 0;
    //omp_set_num_threads(4);
#pragma omp parallel
    {
#pragma omp single nowait
        Gray_modular_nonproportional_by_stack_omp_cosets(Q, N - K, K, br, helper, 1);
    }


    printf("Global covering rad = : %d\n", R);
    for (int i = 0; i < 100; i++) {
        if (coset_leader_WD[i] > 0)
            printf("%d^%d   ", i, coset_leader_WD[i] * (Q - 1));
    }

   

}
//=================================================================
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);
    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
    int threads = omp_get_thread_num();


    time_t t;
    srand((unsigned)time(&t));
    int n = 3, k = 3, q = 3;
    //cout << "Enter n, k, q:";
   // cin >> n >> k >> q;
    //SIZE = 2;
    double start_t, end_t;

    dreadgmatvcodeblock();

    start_t = omp_get_wtime();
    
   // covering_rad_beg_OpenMP_cosets(); //OpenMP cosets
   // covering_rad_beg_MPI();//MPI
    covering_rad_beg_MPI_OMP_cosets();//MPI+OMP(cosets)
   // covering_rad_beg_SSE_only();
    end_t = omp_get_wtime();



    if (RANK == 0){
    //printf("Global covering rad = : %d\n", R);
    //for (int i = 0; i < 100; i++) {
    //    if (coset_leader_WD[i] > 0)
    //        printf("%d^%d   ", i, coset_leader_WD[i] * (Q - 1));
   // }
        printf(" Work took %f seconds\n", end_t - start_t);
    }


    MPI_Finalize();
    return 0;


}
