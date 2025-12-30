# MPI Parallel Implementation for Computing the Covering Radius of Linear Codes

This work presents a Message Passing Interface (MPI) parallel implementation (provided as a .cpp file) of a known algorithm for computing the covering radius of linear $[n,k]\_q$ ​codes $C$ over finite fields, based on a parity-check matrix. The algorithm relies on generating linear combinations of the columns of a parity-check matrix $H_{n-k*n}$ and, consequently, the corresponding syndromes of the code $C$. The parallelization strategy is based on partitioning the ordered set $V_n^L$, which represents all non-proportional linear combinations of up to $L$ columns of $H$. For implementation purposes, ranking and unranking functions are employed to generate ordered subsets of $V_n^L$. An enumeration method for the syndromes of the linear code is also included, as it constitutes a fundamental component of the algorithm.
The MPI parallelization follows a Master–Worker strategy, where a single master process maintains and tracks the set of generated syndromes, while the worker processes perform the computationally intensive tasks of generating linear combinations. Additionally, vectorization based on the SSE4.1 instruction set is employed as part of the linear-combination generation, further accelerating this stage of the algorithm.

#### Parallel Computing 

MPI is a widely adopted standard for parallel programming in distributed-memory systems, enabling scalable execution through explicit message-based communication between processes.
Vectorization complements MPI by exploiting data-level parallelism via CPU vector registers, allowing multiple vector elements to be processed simultaneously and thereby further improving computational performance.

The Master–Worker paradigm in MPI involves a master process that coordinates the computation by distributing tasks, collecting results, and managing termination conditions. Worker processes operate independently, repeatedly receiving tasks from the master, executing them, and returning the results. This paradigm is particularly effective for applications with irregular or dynamically generated workloads, as it enables adaptive load balancing across processes. Moreover, flexible task granularity allows the communication-to-computation ratio to be tuned according to the problem requirements and the underlying hardware.

