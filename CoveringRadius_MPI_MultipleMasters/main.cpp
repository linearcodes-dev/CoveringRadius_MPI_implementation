// Covering Radius of a Linear Code: Main and Additional Functions

// Required headers for MPI communication and basic I/O and data types
#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Project header for generating and handling linear combinations
#include "LinearCombinations.h"

// Use the standard C++ namespace
using namespace std;

#define NEW_TASK 3
#define DONE 4
#define RESULT 5
#define RESULT_DONE 6

#define MASTERN 2 // Set the number of Master processes in the MPI program
#define ARRAY_SIZE 10000 // Define the size buffer array used for MPI communication

//================================================================
// Matrix parameters set by input file
int r = 0;
int k, n, q;
//================================================================
// Power function 
uint64_t ipow(size_t base, int exp)
{
	uint64_t result = 1;
	while (exp > 0) {
		result *= base;
		exp--;
	}
	return result;
}
//================================================================
// Radix sort for 64-bit unsigned integers (byte-wise, using counting sort)
void radix_sort_uint64(unsigned long long int* a, int n)
{
	uint64_t* b = (uint64_t*)malloc(n * sizeof(uint64_t));
	int counts[256];

	// Loop over each of the 8 bytes of the 64-bit number
	for (int shift = 0; shift < 64; shift += 8)
	{
		memset(counts, 0, sizeof(counts));

		// 1. Count occurrences of the current byte
		for (int i = 0; i < n; i++)
		{
			counts[(a[i] >> shift) & 0xFF]++;
		}

		// 2. Compute positions (prefix sums)
		int total = 0;
		for (int i = 0; i < 256; i++)
		{
			int old_count = counts[i];
			counts[i] = total;
			total += old_count;
		}

		// 3. Rearrange elements in the auxiliary array b
		for (int i = 0; i < n; i++)
		{
			b[counts[(a[i] >> shift) & 0xFF]++] = a[i];
		}

		// 4. Copy back to array a (or swap pointers)
		memcpy(a, b, n * sizeof(uint64_t));
	}
	free(b);
}
//================================================================
// Maximum allowed size for rows and columns of the matrix
#define MAX 100
// Transposes a rectangular matrix in-place using a temporary auxiliary matrix.
// After execution, the number of rows and columns in 'matrix' are swapped.
void transpose_rectangular()
{
	int temp_matrix[MAX][MAX];

	int K = matrix.k;
	int N = matrix.n;
	// 1. Copy the transposed values into the auxiliary matrix
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < N; j++)
		{
			temp_matrix[j][i] = matrix.a[i][j];
		}
	}
	// 2. Copy the result back into the original matrix
	// Note: The matrix now has N rows and K columns!
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < K; j++)
		{
			matrix.a[i][j] = temp_matrix[i][j];
		}
	}
	matrix.k = N;
	matrix.n = K;
}
//================================================================
// Reads a k×n matrix from file "dual" into 'matrix', converting characters to integers; returns 0 on success, 1 on error.
int read_matrix()
{
	// Initialize values to 0
	matrix.n = 0;
	matrix.k = 0;
	matrix.q = 0;
	matrix.num = 0;

	FILE* file = fopen("dual", "r");
	if (file == NULL)
	{
		perror("Wrong fail");
		return 1;
	}

	char prefix;
	// Read the header line: expample: ? 10 30 3 3
	// Expample. Assign 10 to n, 30 to k, and the remaining to q and num
	if (fscanf(file, " %c %d %d %d %d", &prefix, &matrix.k, &matrix.n, &matrix.q, &matrix.num) != 5)
	{
		fprintf(stderr, "Wrong format.\n");
		fclose(file);
		return 1;
	}

	// Check for array overflow (the structure is fixed at 100x100)
	if (matrix.n > 100 || matrix.k > 100)
	{
		fprintf(stderr, "Greater than (100x100).\n");
		fclose(file);
		return 1;
	}

	// Read the matrix itself, row by row
	for (int i = 0; i < matrix.k; i++)
	{
		char line[101]; // Temporary buffer for a row as a string

		// Read the next string of characters (skipping spaces and newlines)
		if (fscanf(file, "%100s", line) == 1)
		{
			for (int j = 0; j < matrix.n; j++)
			{
				// Store the character directly ('0', '1', '2') or convert it to a number
				// If you want numbers 0, 1, 2, use: line[j] - '0';
				matrix.a[i][j] = line[j] - '0';
				;
			}
		}
	}
	fclose(file);
	return 0;
}

/* ==========================================================
   MAIN FUNCTION
   ========================================================== */
int main(int argc, char** argv)
{
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	double tg_start, tg_end; // Variables to store start and end times for timing

	int myRank, nprocs; // Rank and total processes
	// Get this process's rank and total number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	//================================================================
	//Number processes condition check. At least one worker process is required! 
	if (nprocs <= MASTERN)
	{
		int num_workers = nprocs - MASTERN;
		if (num_workers < 0)
			num_workers = 0;
		if (myRank == 0)
		{
			printf("\nNumber of processes:%d Number of Master processes:%d Number of Worker processes:%d", nprocs, MASTERN, num_workers);
			printf("\nError: At least one worker process is required!\n\nExit!\n\n");
		}
		MPI_Finalize();
		return 0;
	}
	else {
		int num_workers = nprocs - MASTERN;
		if (myRank == 0)
		{
			printf("\nNumber of processes:%d Number of Master processes:%d Number of Worker processes:%d\n\n", nprocs, MASTERN, num_workers);
		}
	}
	//================================================================
	read_matrix(); // Initialize and load matrix data from file "dual"

	n = matrix.n;
	k = matrix.k;
	q = matrix.q;

	if (myRank == 0) { printf("Open input file 'dual' with parameters.\nn:%d k:%d q:%d \n", n, k, q); }

	maketable(q);
	maketableover(n, n, q);

	MPI_Status status;
	//================================================================
	// Create output file name for rank process 
	//char Outfilename[20];
	//sprintf(Outfilename, "%d_cpu_console.txt", myRank);
	//================================================================
	bool first = false;
	unsigned long long int total_lc = 1;
	unsigned long long int hamming = HemmingBound(n, n - k, q);
	if (myRank == 0) { printf("\nLower radius, R=%llu\n", hamming); }

	unsigned long long int total;
	power(q, k);
	total = (QN[k] - 1) / (q - 1);						//(power(q, k) - 1) / (q - 1);
	unsigned long long int masterbeg[MASTERN], masterend[MASTERN], totalm[MASTERN];
	unsigned long long int permast = total / MASTERN;
	if (1 == MASTERN)
	{
		masterbeg[0] = 1;
		masterend[0] = total;
		totalm[0] = total;
	}
	else
	{
		for (int i = 0; i < MASTERN; i++)
		{
			masterbeg[i] = i * permast + 1;
			masterend[i] = (i + 1) * permast;
			totalm[i] = masterend[i] - masterbeg[i] + 1;
			if ((MASTERN - 1) == i)
			{
				masterbeg[i] = masterend[i - 1] + 1;
				masterend[i] = total;
				totalm[i] = masterend[i] - masterbeg[i] + 1;
			}
		}
	}


	MPI_Comm_rank(MPI_COMM_WORLD, &myRank); // Get the rank of the process
	tg_start = MPI_Wtime(); // Start time for timing measurement
	//================================================================
	//   MASTER PROCESS
	//================================================================
	if (myRank < MASTERN)
	{
		int terminate = 0;
		//================================================================
		// Dynamically allocate memory for the syndrome array
		int rr = n - k;
		const unsigned long long int synLenght = ipow(q, rr) / 64 + 10; //const unsigned long long int synLenght = 1500000; 
		unsigned long long int* syn;
		syn = (unsigned long long int*)malloc((synLenght) * sizeof(unsigned long long int));
		if (syn == NULL)
		{
			fprintf(stderr, "Master failed to allocate memory!\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		// Initialize all elements of the syndrome array to zero
		for (int i = 0; i < synLenght; i++) { syn[i] = 0; }
		//================================================================
		int ki = (int)hamming;

		total_lc = sumover(n, ki);

		unsigned long long int count_diff = 0; // Count distinct syndromes for each linear combination

		if (ki == hamming)
		{
			first = true;
		}
		else
		{
			first = false;
		}
		int workers_done = 0;
		int flag = 0;

		while (!terminate)
		{
			// Non-blocking probe for any incoming message from any source and any tag
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

			if (flag)
			{
				int sender = status.MPI_SOURCE;

				if (status.MPI_TAG == RESULT_DONE) // Check for end-of-results message
				{
					MPI_Recv(NULL, 0, MPI_INT, sender, RESULT_DONE, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive end-of-results signal
					workers_done++;

					if (workers_done == (nprocs - MASTERN))
					{
						workers_done = 0;
						if (count_diff >= totalm[myRank])
						{
							r = ki;
							terminate = 1;
						}
						else
						{
							ki++;
						}
						for (int i = MASTERN; i < nprocs; i++)
						{
							MPI_Send(&terminate, 1, MPI_INT, i, NEW_TASK, MPI_COMM_WORLD); // Notify process i to terminate
						}
						if (1 == terminate)
						{
							break;
						}
					}
				}

				if (status.MPI_TAG == RESULT) // Check for result message
				{
					static unsigned long long int buffer[ARRAY_SIZE];
					MPI_Recv(buffer, ARRAY_SIZE, MPI_UNSIGNED_LONG_LONG, sender, RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive result array from sender

					for (int i = 0; i < ARRAY_SIZE; i++)
					{

						unsigned long long int id = buffer[i] /*-masterbeg[myRank]*/;
						if (id > 0) {
							id = id - masterbeg[myRank] + 1;
						}

						unsigned long long int ii = 0, diff = 0;
						unsigned long long int ONE = 1;
						unsigned long long int temp = id % 64;
						ii = id >> 6;						   // ii = (id / 64);
						unsigned long long int test = (ONE << (63 - temp));
						if (id > 0)
							if (!(syn[ii] & test))
							{
								count_diff++;
								syn[ii] = syn[ii] | test; //(ONE << (63 - temp));
							}
					}
				}
			}
		}
		// Deallocate syndrome array
		free(syn);
	}

	// ==========================================================
	//   WORKERS PROCESSES
	// ==========================================================
	if (MASTERN <= myRank)
	{
		static unsigned long long int arr[MASTERN][ARRAY_SIZE];
		//============================================
		//printMatrix(false, Outfilename); // Write readed matrix into file
		//============================================
		int donealll[MASTERN];

		for (int i = 0; i < MASTERN; i++)
		{
			donealll[i] = 0;
		}

		power(q, k);
		Characteristic = q;
		transpose_rectangular();
		setRegistersBytes(bitsCharCF, n, k, q, 1); // bitsCharCF==bits
		//============================================
		// Working with the transposed matrix, Swap the values of n and k, 
		int ch = n;
		n = k;
		k = ch;
		//============================================
		int ki = (int)hamming;
		int idh = 0;

		unsigned long long int beginRank = 1, perth = total_lc;
		int p = (nprocs - MASTERN);
		int terminate = 0;

		while (1)
		{
			if (ki == hamming)
			{
				first = true;
			}
			else
			{
				first = false;
			}
			int terminateall = 0;
			total_lc = 1;

			total_lc = (sumover(k, ki) - 1) / (q - 1);
			perth = total_lc / p;
			beginRank = (myRank - MASTERN) * perth + 1;

			if (myRank == (nprocs - 1))
			{
				perth = perth + total_lc % p;
			}
			// Generate the vector representing the linear combination for the given rank
			unrankBBgen(beginRank, q, k, ki);

			unsigned long long int count = 0, kb = 1;
			for (int i = 0; i < K; i++)
			{
				coef[i] = 0;
				lc[i] = 0;
			}
			for (int i = 1; i <= k; i++)
			{
				if (CC[i] != 0)
					kb = i;
			}
			int q_fin = q - 1;
			int j = 1, i = 1;
			// Set up helper matrix and parameters for computing the linear combinations before the first one
			for (i = 1; i < kb; i++)
			{
				if (CC[i] > 0)
				{
					lc[j] = i;
					coef[j] = CC[i];
					add(j - 1, lc[j], j, n);
					for (int ll = 0; ll < coef[j] - 1; ll++)
					{
						add(j, lc[j], j, n);
					}
					j++;
				}
			}

			lc[j] = i;
			coef[j] = CC[i];
			if (coef[j] > 1)
			{
				add(j - 1, lc[j], j, n);
				for (int ll = 0; ll < coef[j] - 2; ll++)
				{
					add(j, lc[j], j, n);
				}
			}
			j++;
			j--;
			coef[j]--;

			i = 1;
			unsigned int counter[MASTERN];
			for (int i = 0; i < MASTERN; i++)
			{
				counter[i] = 0;
			}
			// Generate the first linear combination
			for (int h = lc[1]; h <= k; h++)
			{
				if (lc[1] != h)
				{
					lc[1] = h;
					j = 1;
					coef[1] = 0;
				}
				do
				{
					if (coef[j] < q_fin)
					{
						coef[j]++;
					}
					else
					{
						lc[j]++;
						coef[j] = 1;
					}

					if (coef[j] == 1)
					{
						add(j - 1, lc[j], j, n);
					}
					else
					{
						add(j, lc[j], j, n);
					}
					count++;

					if ((first) || (j == ki))
					{
						unsigned long long int id = pointtointi_lr(helper_p[j], n);

						if (0 == id)
							id = 1;
						unsigned long long int idhh = (id - 1) / totalm[0];
						idh = (int)idhh;

						if (idh >= MASTERN)
						{
							idh = MASTERN - 1;
						}

						arr[idh][counter[idh]] = id;
						counter[idh]++;
						if (counter[idh] > (ARRAY_SIZE - 1))
						{
							if (0 == donealll[idh]) {
								MPI_Send(arr[idh], ARRAY_SIZE, MPI_UNSIGNED_LONG_LONG, idh, RESULT, MPI_COMM_WORLD); // Send result array to process idh
								counter[idh] = 0;
							}
						}
					}

					if (lc[j] < k)
					{
						if (j < ki)
						{
							// Ensure linear combination of the i-th row out of k total rows => j < i
							j++;
							lc[j] = lc[j - 1] + 1;
							if (coef[j] == q_fin)
							{
								coef[j] = 0;
							}
						}
					}
					else if (coef[j] == q_fin)
					{
						j--;
					}

				} while (count < perth && j != 1); //(j != 1);

				if (count >= perth)
				{
					for (int m = 0; m < MASTERN; m++)
					{
						if (0 == donealll[m])
						{
							for (int ij = counter[m]; ij < ARRAY_SIZE; ij++)
							{
								arr[m][ij] = 0;
							}
							MPI_Send(arr[m], ARRAY_SIZE, MPI_UNSIGNED_LONG_LONG, m, RESULT, MPI_COMM_WORLD); // Send result array to process m
							counter[m] = 0;
						}
					}
					break;
				}
			}

			terminateall = MASTERN;
			for (int i = 0; i < MASTERN; i++)
			{
				if (0 == donealll[i])
				{
					MPI_Send(NULL, 0, MPI_INT, i, RESULT_DONE, MPI_COMM_WORLD);
					terminateall--;
				}
			}

			int flag, recMASTER = 0;
			while ((MASTERN - terminateall) != recMASTER)
			{
				// Non-blocking check for any incoming MPI message from any source or tag
				MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

				if (flag)
				{
					int sender = status.MPI_SOURCE;
					MPI_Recv(&donealll[sender], 1, MPI_INT, sender, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive status value from sender
					recMASTER++;
				}
			}

			terminateall = 0;
			for (int m = 0; m < MASTERN; m++)
			{
				if (1 == donealll[m])
				{
					terminateall++;
				}
			}

			if (MASTERN == terminateall)
			{
				break;
			}
			else
			{
				ki++;
			}
		}
	}

	int R_final = 0;
	MPI_Reduce(&r, &R_final, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD); // MPI reduction to find global maximum of 'r'
	tg_end = MPI_Wtime(); // End time for timing measurement

	MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes in MPI_COMM_WORLD
	if (myRank == 0) { printf("\nCovering radius, R=%d\n", R_final); } // Master process prints the final covering radius

	if (myRank < MASTERN)
	{
		// Print total execution time for the master process
		printf("\nMASTER, Rank=%d total execution time: %.3f s \n", myRank, (tg_end - tg_start));
	}
	MPI_Finalize(); // Finalize the MPI environment and clean up resources
	return 0;
}
