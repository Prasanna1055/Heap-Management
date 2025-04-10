# 🧠 Heap Management in C

A C program that simulates a basic heap management system with custom `allocate()` and `free()` functions. Mimics the behavior of dynamic memory allocation by managing a manually allocated memory region.

## 🚀 Features

- Simulates dynamic memory allocation like `malloc()` and `free()`
- Maintains a free list of memory blocks with block headers
- Automatically merges adjacent free blocks to reduce fragmentation

## 📁 Structure

- `allocate(size_t size)`: Allocates a block of the given size from the heap
- `free(void *ptr)`: Frees the block and merges adjacent free memory
- Heap simulated using a static memory array

## 🛠️ Technologies

- Language: C
- Concepts: Manual memory management, pointer arithmetic, block merging

## 📦 Compile & Run

```bash
gcc heap.c -o heap
./heap
