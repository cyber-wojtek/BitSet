# Sieve of Erastothenes
All of time measurements are measured with [example_primes_time_windows.cpp](https://github.com/cyber-wojtek/BitSet_C_Cpp_Python/blob/main/Examples/example_primes_time_windows.cpp)
## Classic Implementation using Bool Pointer Array
Below is classic, efficient implementation of Sieve of Erastothenes, using bool pointer array.
```cpp
template <typename T>
inline std::enable_if_t<std::is_integral_v<T>, std::pair<T*, std::size_t>> primes_sieve_of_eratosthenes(const T& up_limit, const bool& use_prime_num_approx = false, const uint64_t& mem_to_alloc = 0)
{
    // If limit is less than 2, return empty array
    if (up_limit < 2)
        return {nullptr, 0};

    // Initialize variables
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
```

This function calculates all primes up to 100000000 in `0.4088639638` seconds.

## Implementation using CDynamicBitSet
Function below uses CDynamicBitSet class to store primes
```cpp
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
```

This function calculates all primes up to 100000000 in `0.2169543829` seconds, which is about 1.88x faster.
