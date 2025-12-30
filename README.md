# MPI Parallel Implementation for Computing the Covering Radius of Linear Codes

This work presents a Message Passing Interface (MPI) parallel implementation (provided as a .cpp file) of a known algorithm for computing the covering radius of linear $[n,k]\_q$ ​codes $C$ over finite fields, based on a parity-check matrix. The algorithm relies on generating linear combinations of the columns of a parity-check matrix $H_{n-k*n}$ and, consequently, the corresponding syndromes of the code $C$. The parallelization strategy is based on partitioning the ordered set $V_n^L$, which represents all non-proportional linear combinations of up to $L$ columns of $H$. For implementation purposes, ranking and unranking functions are employed to generate ordered subsets of $V_n^L$. An enumeration method for the syndromes of the linear code is also included, as it constitutes a fundamental component of the algorithm.
The MPI parallelization follows a Master–Worker strategy, where a single master process maintains and tracks the set of generated syndromes, while the worker processes perform the computationally intensive tasks of generating linear combinations. Additionally, vectorization based on the SSE4.1 instruction set is employed as part of the linear-combination generation, further accelerating this stage of the algorithm.

#### Parallel Computing 

MPI is a widely adopted standard for parallel programming in distributed-memory systems, enabling scalable execution through explicit message-based communication between processes.
Vectorization complements MPI by exploiting data-level parallelism via CPU vector registers, allowing multiple vector elements to be processed simultaneously and thereby further improving computational performance.

The Master–Worker paradigm in MPI involves a master process that coordinates the computation by distributing tasks, collecting results, and managing termination conditions. Worker processes operate independently, repeatedly receiving tasks from the master, executing them, and returning the results. This paradigm is particularly effective for applications with irregular or dynamically generated workloads, as it enables adaptive load balancing across processes. Moreover, flexible task granularity allows the communication-to-computation ratio to be tuned according to the problem requirements and the underlying hardware.

##### Vectorization

The proposed parallel procedure accelerates the computation of the covering radius by efficiently evaluating linear combinations of parity-check matrix columns over finite fields $F_q$, with $q≤64$, using SIMD vectorization. By packing column vectors into 128-bit registers and performing byte-wise modular addition entirely within SIMD instructions, the implementation avoids lookup tables, minimizes memory access, and preserves exact arithmetic. As a result, the SIMD-enhanced approach significantly reduces computational overhead and enables scalable exploration of larger search spaces in practical time.

#### Parallel implementation

Parallelizing the algorithm for computing the covering radius of a linear code presents challenges related to synchronization and shared data access, particularly for managing the syndrome array. The proposed MPI-based Master–Worker approach addresses these issues by assigning the generation of linear combinations to worker processes while the master process coordinates iterations over the parameter $l$, collects newly generated syndromes, updates the global syndrome array, and determines termination once all syndromes have been covered. The master process is given in Algorithm:

...

### Experimental results

...

### Note
More details about the implemented algorithms, their performance analysis, and experimental results can be found in the following paper:
Bouyukliev I, Bikov D, Pashinska-Gadzheva M. How to Find the Covering Radius of Linear Codes over Finite Fields Using a Parity-Check Matrix in Parallel.
