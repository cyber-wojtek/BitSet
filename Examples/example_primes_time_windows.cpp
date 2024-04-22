// for std::memset
#include <cstring>

// for std::log
#include <cmath>

// for shorter type names
#include <cstdint>

// for C++ I/O
#include <iostream>
#include <iomanip>

// for std::pair
#include <utility>

// for timing functions (could use std::chrono, but this is more accurate)
#include <Windows.h>

// adjust as needed
#include "BitSet.h"
#include "BitSet.hpp"

template <typename T>
inline std::enable_if_t<std::is_integral_v<T>, std::pair<T*, std::size_t>> primes_sieve_of_eratosthenes_bitset_c(const T& up_limit, const bool& use_prime_num_approx, const uint64_t& mem_to_alloc)
{
    // If limit is less than 2, return empty array
    if (up_limit < 2)
        return {nullptr, 0};

    // Initialize variables
    // use BitSet class to save memory (and time) for large limits (up to 8 times less memory usage and 8 times faster access)

    BitSet primes_b;
    bit_set_init(&primes_b, up_limit + 1);

    T primes_size = (mem_to_alloc) ? mem_to_alloc : ((use_prime_num_approx) ? static_cast<T>(up_limit / std::log(up_limit)) : up_limit);
    T* primes = new T[primes_size];
    *primes = 2;
    T index = 1;

    // setting the first two elements to 'false' (not prime) and the third element to 'true' (prime).

    // All other even numbers are not primes (optimization), using pointer arithmetic to set bits
    bit_set_fill_chunk_in_range_begin_end(&primes_b, 0b10101010, 1, primes_b.storage_size);
    bit_set_set_chunk(&primes_b, 0b10101100, 0);
    //std::fill(bits + 1, bits + up_limit / 64 + 1, 0b1010101010101010101010101010101010101010101010101010101010101010);
    
    // Main sieve loop
    T i = 3;
    for (; i * i <= up_limit; i += 2)
    {
        if (bit_set_get(&primes_b, i))
        {
            *(primes + index++) = i;
            /*for (T j = i * i; j <= up_limit; j += i)
                primes_b.set(false, j);*/
            bit_set_clear_in_range_begin_end_step(&primes_b, i * i, up_limit, i);
        }
    }

    // Copy the remaining primes
    if (!(i % 2))
        ++i;
    for (; i <= up_limit; i += 2)
    {
        if (bit_set_get(&primes_b, i))
            *(primes + index++) = i;
    }

    bit_set_destroy(&primes_b);

    // Return primes and their count
    return { primes, primes_size };
}

template <typename T>
inline std::enable_if_t<std::is_integral_v<T>, std::pair<T*, std::size_t>> primes_sieve_of_eratosthenes_bitset_cpp(const T& up_limit, const bool& use_prime_num_approx, const uint64_t& mem_to_alloc)
{
    // If limit is less than 2, return empty array
    if (up_limit < 2)
        return {nullptr, 0};

    // Initialize variables
    // use BitSet class to save memory (and time) for large limits (up to 8 times less memory usage and 8 times faster access)

    CDynamicBitSet<uint8_t> primes_b(up_limit + 1);

    T primes_size = (mem_to_alloc) ? mem_to_alloc : ((use_prime_num_approx) ? static_cast<T>(up_limit / std::log(up_limit)) : up_limit);
    T* primes = new T[primes_size];
    *primes = 2;
    T index = 1;

    // setting the first two elements to 'false' (not prime) and the third element to 'true' (prime).

    // All other even numbers are not primes (optimization), using pointer arithmetic to set bits
    primes_b.fill_chunk_in_range(0b10101010, 1, primes_b.storage_size);
    primes_b.set_chunk(0b10101100, 0);
    //std::fill(bits + 1, bits + up_limit / 64 + 1, 0b1010101010101010101010101010101010101010101010101010101010101010);
    
    // Main sieve loop
    T i = 3;
    for (; i * i <= up_limit; i += 2)
    {
        if (primes_b.get(i))
        {
            *(primes + index++) = i;
            /*for (T j = i * i; j <= up_limit; j += i)
                primes_b.set(false, j);*/
            primes_b.clear_in_range(i * i, up_limit, i);
        }
    }

    // Copy the remaining primes
    if (!(i % 2))
        ++i;
    for (; i <= up_limit; i += 2)
    {
        if (primes_b.get(i))
            *(primes + index++) = i;
    }

    // Return primes and their count
    return { primes, primes_size };
}

/*
 * Sieve of Eratosthenes algorithm for finding all primes up to a given limit.
 * 
 * Arguments:
 *  - up_limit: The upper limit for the primes.
 *  - use_prime_num_approx: If true, use the prime number approximation (pi(x) <= x / ln(x)) to calculate the size of the array to store the primes.
 *                          Otherwise, use the upper limit as the size of the array.
 *  - mem_to_alloc: (Optional) The amount of memory to allocate for the array of primes.
 *                  If not provided (or 0), the function will allocate memory based on the specified limit or approximation.
 * 
 * Returns:
 *  - A pair consisting of a pointer to the array of primes and the number of primes found.
 */

template <typename T>
inline std::enable_if_t<std::is_integral_v<T>, std::pair<T*, std::size_t>> primes_sieve_of_eratosthenes(const T& up_limit, const bool use_prime_num_approx, const uint64_t mem_to_alloc)
{
    // If limit is less than 2, return empty array
    if (up_limit < 2)
        return {nullptr, 0};

    // Initialize variables
    // use BitSet class to save memory (and time) for large limits (up to 8 times less memory usage and 8 times faster access)
    bool *primes_b = new bool[up_limit + 1];
    T primes_size = (mem_to_alloc) ? mem_to_alloc : ((use_prime_num_approx) ? static_cast<T>(up_limit / std::log(up_limit)) : up_limit);
    T* primes = new T[primes_size];
    *primes = 2;
    T index = 1;

    // setting the first two elements to 'false' (not prime) and the third element to 'true' (prime).
    *primes_b = *(primes_b + 1) = false;
    *(primes_b + 2) = true;

    std::memset(primes_b, true, up_limit + 1);

    //std::fill(bits + 1, bits + up_limit / 64 + 1, 0b1010101010101010101010101010101010101010101010101010101010101010);
    
    // Main sieve loop
    T i = 3;
    for (; i * i <= up_limit; i += 2)
    {
        if (*(primes_b + i))
        {
            *(primes + index++) = i;
            for (T j = i * i; j <= up_limit; j += i)
                *(primes_b + j) = false;
        }
    }

    // Copy the remaining primes
    if (!(i % 2))
        ++i;
    for (; i <= up_limit; i += 2)
    {
        if (*(primes_b + i))
            *(primes + index++) = i;
    }


    delete[] primes_b;

    // Return primes and their count
    return { primes, primes_size };
}
int main()
{
    // Disable synchronization between C and C++ standard streams
    //std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    // Enable virtual terminal processing for ANSI escape codes
    DWORD org_mode;
    GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &org_mode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), org_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    std::cout << std::fixed << std::setprecision(10);

    constexpr uint64_t am_runs = 10000000, up_limit = 100000000ull;

    uint64_t *primes, primes_count;
    LARGE_INTEGER start, end, freq;
    ULONGLONG start_time, end_time;

    long double pf_avg_time = 0,
        tc_avg_time = 0,
        pf_avg_time_bitset_c = 0,
        tc_avg_time_bitset_c = 0,
        pf_avg_time_bitset_cpp = 0,
        tc_avg_time_bitset_cpp = 0;
    
    QueryPerformanceFrequency(&freq);

    // set high priority for the process
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    for (uint64_t i = 0; i < am_runs; ++i)
    {
        if (i)
            std::cout << "\033[4A";
        std::cout << "Iteration: " << i + 1 << '\n';

        // include 1st one least optimized
        {
            start_time = GetTickCount64();
            QueryPerformanceCounter(&start);
            std::tie(primes, primes_count) = primes_sieve_of_eratosthenes(up_limit, false, 0);
            QueryPerformanceCounter(&end);
            end_time = GetTickCount64();
            delete[] primes;
        }
        pf_avg_time += (end.QuadPart - start.QuadPart) / static_cast<long double>(freq.QuadPart);
        tc_avg_time += end_time - start_time;

        {
            start_time = GetTickCount64();
            QueryPerformanceCounter(&start);
            std::tie(primes, primes_count) = primes_sieve_of_eratosthenes_bitset_c(up_limit, false, 0);
            QueryPerformanceCounter(&end);
            end_time = GetTickCount64();
            delete[] primes;
        }
        pf_avg_time_bitset_c += (end.QuadPart - start.QuadPart) / static_cast<long double>(freq.QuadPart);
        tc_avg_time_bitset_c += end_time - start_time;

        {
            start_time = GetTickCount64();
            QueryPerformanceCounter(&start);
            std::tie(primes, primes_count) = primes_sieve_of_eratosthenes_bitset_cpp(up_limit, false, 0);
            QueryPerformanceCounter(&end);
            end_time = GetTickCount64();
            delete[] primes;
        }
        pf_avg_time_bitset_cpp += (end.QuadPart - start.QuadPart) / static_cast<long double>(freq.QuadPart);
        tc_avg_time_bitset_cpp += end_time - start_time;

        std::cout << "Average time for sieve of eratosthenes:                   " << tc_avg_time / (i + 1) / 1000 << ", " << pf_avg_time / (i + 1) << '\n'
            << "Average time for sieve of eratosthenes (bitset, C API):   " << tc_avg_time_bitset_c / (i + 1) / 1000 << ", " << pf_avg_time_bitset_c / (i + 1) << '\n'
            << "Average time for sieve of eratosthenes (bitset, C++ API): " << tc_avg_time_bitset_cpp / (i + 1) / 1000 << ", " << pf_avg_time_bitset_cpp / (i + 1) << '\n';
    }
    
    std::cout << "\033[3A";

    std::cout << "Average time for sieve of eratosthenes:                   " << tc_avg_time / am_runs / 1000 << ", " << pf_avg_time / am_runs << '\n';
    std::cout << "Average time for sieve of eratosthenes (bitset, C API):   " << tc_avg_time_bitset_c / am_runs / 1000 << ", " << pf_avg_time_bitset_c / am_runs << '\n';
    std::cout << "Average time for sieve of eratosthenes (bitset, C++ API): " << tc_avg_time_bitset_cpp / am_runs / 1000 << ", " << pf_avg_time_bitset_cpp / am_runs << '\n';


    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), org_mode);
    return 0;
}
