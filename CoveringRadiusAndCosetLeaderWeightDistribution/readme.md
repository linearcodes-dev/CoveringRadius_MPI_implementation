# CoveringRadiusCosets

A High-Performance Parallel Computing tool implemented in C++ for computing the covering radius and coset leader weight distribution of linear codes over finite fields ($GF(q)$). The project leverages SIMD instructions (AVX2/SSE), OpenMP multi-threading, and MPI distributed processing to scale calculations across HPC architectures.

---

## Features

* **Hybrid Parallelism**: Combines distributed computing via MPI with multi-core thread parallelism via OpenMP.
* **SIMD Optimization**: Utilizes SIMD intrinsics (`popcnt`, AVX2, SSE) for accelerated vector addition and weight calculation.
* **Efficient Exploration**: Uses modular non-proportional Gray codes to systematically iterate over coset representatives.
* **Flexible Execution**: Flexible modes for running with Pure SIMD, Pure MPI, Pure OpenMP, or Hybrid MPI+OpenMP.

---

## Prerequisites

* **C++ Compiler**: C++17 or later supporting AVX2 or SSE vector intrinsics (GCC, Clang, or MSVC).
* **Build System**: CMake (v3.8 or higher).
* **MPI Library**: OpenMPI, MPICH, or MS-MPI.
* **OpenMP**: Supported by most standard compilers (`-fopenmp` / `/openmp`).

---

## Building the Project

1. **Clone or navigate to the project directory:**
   ```bash
   cd CoveringRadiusCosets
   ```

2. **Generate build files and compile:**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

---

## Usage

### 1. Matrix Input File Setup
The program reads input generator matrices from a file named `EXAM` in the working directory where the executable runs.

Format requirement:
```text
? <K> <N> <Q> <ID>
<Generator Matrix Rows>
```
* `K`: Dimension of the code (number of rows).
* `N`: Code length (number of columns).
* `Q`: Order of the finite field $GF(q)$.

#### Example `EXAM` File:
```text
? 3 6 2 1
100110
010101
001011
```

### 2. Execution

* **Run via MPI:**
  ```bash
  mpirun -np 4 ./CoveringRadiusCosets
  ```

* **Set OpenMP Thread Count:**
  ```bash
  export OMP_NUM_THREADS=8
  mpirun -np 4 ./CoveringRadiusCosets
  ```

---

## Output

The program outputs execution metrics to stdout upon completion:
* **Global Covering Radius ($R$)**: The maximum distance from any vector in $GF(q)^n$ to the nearest codeword.
* **Coset Leader Weight Distribution**: Formatted output showing weight frequencies ($W^C$).
* **Execution Time**: Total duration spent computing.

---

## Citation & Reference

This implementation is based on research on parallel algorithms for linear codes. If you use this software in academic work, please cite:

* **Paper**: Pashinska-Gadzheva, M., Bouyukliev, I. (2026). About Parallelization of Algorithm for Computation of Covering Radius of Linear Codes. In: Lirkov, I., Margenov, S. (eds) Large-Scale Scientific Computations. LSSC 2025. Lecture Notes in Computer Science, vol 16061. Springer, Cham. https://doi.org/10.1007/978-3-032-22221-3_45
