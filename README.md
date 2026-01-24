# MPI Parallel Implementation for Computing the Covering Radius of Linear Codes

This work presents a Message Passing Interface (MPI) parallel implementation (provided as a $.cpp$ file, together with headers $.h$ files, in a separate directory) of a algorithm for computing the covering radius of linear $[n,k]\_q$ ​codes $C$ over finite fields, based on a parity-check matrix. The algorithm relies on generating linear combinations of the columns of a parity-check matrix $H_{n-k*n}$ and, consequently, the corresponding syndromes of the code $C$. The parallelization strategy is based on partitioning the ordered set $V_n^L$, which represents all non-proportional linear combinations of up to $L$ columns of $H$. For implementation purposes, ranking and unranking functions are employed to generate ordered subsets of $V_n^L$. An enumeration method for the syndromes of the linear code is also included, as it constitutes a fundamental component of the algorithm.
The MPI parallelization follows a Master–Worker strategy, where a single (or multiple) master process maintains and tracks the set of generated syndromes, while the worker processes perform the computationally intensive tasks of generating linear combinations. Additionally, vectorization based on the SSE4.1 instruction set is employed as part of the linear-combination generation, further accelerating this stage of the algorithm.

#### Parallel Computing 

MPI is a widely adopted standard for parallel programming in distributed-memory systems, enabling scalable execution through explicit message-based communication between processes.
Vectorization complements MPI by exploiting data-level parallelism via CPU vector registers, allowing multiple vector elements to be processed simultaneously and thereby further improving computational performance.

The Master–Worker paradigm in MPI involves a master process that coordinates the computation by distributing tasks, collecting results, and managing termination conditions. Worker processes operate independently, repeatedly receiving tasks from the master, executing them, and returning the results. This paradigm is particularly effective for applications with irregular or dynamically generated workloads, as it enables adaptive load balancing across processes. Moreover, flexible task granularity allows the communication-to-computation ratio to be tuned according to the problem requirements and the underlying hardware.

##### Vectorization

The proposed parallel procedure accelerates the computation of the covering radius by efficiently evaluating linear combinations of parity-check matrix columns over finite fields $F_q$, with $q≤64$, using SIMD vectorization. By packing column vectors into 128-bit registers and performing byte-wise modular addition entirely within SIMD instructions, the implementation avoids lookup tables, minimizes memory access, and preserves exact arithmetic. As a result, the SIMD-enhanced approach significantly reduces computational overhead and enables scalable exploration of larger search spaces in practical time.

#### Parallel implementation

Parallelizing the algorithm for computing the covering radius of a linear code requires careful handling of synchronization and shared data access, particularly for managing the syndrome array. The proposed MPI-based Master–Worker approach assigns the generation of linear combinations to worker processes, while the master process coordinates iterations over the parameter $l$ (the number of columns of $H$ in each linear combination), collects newly generated syndromes, updates the global syndrome array, and terminates the computation once all $θ$ syndromes are found. The master process is given in Algorithm:

<img width="603" height="366" alt="MasterProcess" src="https://github.com/user-attachments/assets/d887a31c-3592-4157-ba1c-0a140f2651cd" />

Each worker computes a chunk of linear combinations, using SSE4.1 vectorization to accelerate arithmetic over prime fields, and generates syndromes. Newly discovered syndromes are sent back to the master in small batches, which reduces communication overhead and idle time. This design balances the workload across workers and minimizes synchronization delays. The outline of the worker processes workflow is given in Algorithm:

<img width="603" height="494" alt="WorkerProcess" src="https://github.com/user-attachments/assets/b861e6c4-60de-46df-a106-532560f91945" />

In the proposed parallel approach, each worker independently determines the subset of linear combinations it processes based on its rank and the total number of workers, while the master only broadcasts the current radius $R$ and maintains a global syndrome array. Workers enumerate their assigned combinations, compute syndromes, and send newly discovered syndrome indices to the master in buffered batches to reduce communication overhead. This design achieves good scalability and low memory usage, with parallel time complexity 
$O(TOTAL/P)$ and modest communication cost when an appropriate buffer size is used. Only the master stores the global syndrome array, while each worker requires minimal local memory.

##### Master-Worker strategy, multiple master processes

...

### Experimental results

Preliminary experimental results are presented to evaluate the efficiency of the proposed parallel MPI implementation. The computations were executed on an Intel Core i9-12900K processor (3.2 GHz base clock, 16 cores, 24 threads). 

Compile: **mpicxx -O3 -DNDEBUG name.cpu -o name**

Run: **mpirun -np x ./name** 

The MPI parallel implementation employs a Master–Worker strategy. Table 1 presents the execution times for varying numbers of worker processes with a buffer size of 1000000 elements. The implementation also utilizes SSE extended registers.

|  q  |  n  |  k  | R |   W=1   |  W=2   |  W=4   |  W=8   |  W=16  | 
|-----|-----|-----|---|---------|--------|--------|--------|--------|
|  3  |  38 |  22 | 7 |  19.03s |  9.80s |  5.02s |  3.01s |  2.96s |
|  3  |  38 |  21 | 8 | 171.79s | 88.10s | 45.17s | 25.93s | 23.91s |
|  5  |  26 |  15 | 6 |   4.36s |  2.22s |  1.17s |  0.72s |  0.70s |
|  5  |  26 |  14 | 7 |  52.65s | 27.11s | 13.96s |  9.18s |  9.17s |
|  7  |  36 |  27 | 5 |   7.57s |  3.93s |  1.99s |  1.11s |  0.91s |
|  7  |  36 |  26 | 6 | 253.92s | 130.3s | 67.67s | 49.65s | 48.92s |

**Table 1**. Execution times with MPI and Master-Worker strategy

Table 2 presents the computational times for the MPI Master-Worker strategy implementation with multiple master processes. The computations were executed on a Fujitsu Primergy RX2540 M4 system equipped with 128 GB of RAM and two Intel Xeon Gold 5118 processors (2.30 GHz), providing a total of 24 cores. The column labeled $M$ indicates the number of master processes, followed by columns reporting the execution times in seconds for 1, 2, 4, 8, 16, and 24 worker processes.

|  q  |  n  |  k  | R | M |  W=1   |  W=2   |  W=4   |  W=8   |  W=12  |  W=16  |  W=24  |W1(M1)vs.W(24)| 
|-----|-----|-----|---|---|--------|--------|--------|--------|--------|--------|--------|--------------|
|  3  |  38 |  22 | 7 | 1 | 47.15s | 20.04s | 10.46s |  5.49s |  4.59s |  4.69s |  5.60s |     8.41x    |
|  3  |  38 |  22 | 7 | 2 | 48.64s | 25.27s | 13.25s |  7.38s |  4.62s |  3.78s |  3.16s |     14.9x    |   
|  3  |  38 |  22 | 7 | 4 | 51.23s | 25.80s | 13.34s |  7.29s |  4.59s |  3.68s |  3.88s |     12.2x    |    
|  5  |  26 |  14 | 7 | 1 | 139.9s | 57.55s | 29.57s | 16.03s | 16.06s | 16.72s | 17.44s |     8.02x    | 
|  5  |  26 |  14 | 7 | 2 | 141.9s | 72.06s | 38.20s | 20.14s | 13.53s | 11.11s |  9.74s |     14.4x    |
|  5  |  26 |  14 | 7 | 4 | 149.8s | 76.06s | 38.25s | 19.96s | 13.62s | 10.80s |  9.52s |     14.7x    |

**Table 2**. Execution times with MPI and multiple master processes

Speedup ($S_p$) is given by the formula:

$S_p=T_1/T_P$

$P=$ number of processors; $T_1=$ time for optimal serial algorithm on one processor; $T_p$= time for parallel algorithm on $P$ processors.

### Note
More details about the implemented algorithms, their performance analysis, and experimental results can be found in the following paper:
Bouyukliev I, Bikov D, Pashinska-Gadzheva M. How to Find the Covering Radius of Linear Codes over Finite Fields Using a Parity-Check Matrix in Parallel.
