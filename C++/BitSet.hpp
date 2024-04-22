#pragma once
#include <algorithm>
#include <cstdint>
#include <limits>
#include <iostream>
#include <concepts>
#include <new>
#include <cstring>

// Note: (std::numeric_limits<T>::max)() is used instead of std::numeric_limits<T>::max() because Windows.h defines a macro max which conflicts with std::numeric_limits<T>::max()

template <typename T>
concept UnsignedInteger = std::is_unsigned_v<T> && std::is_integral_v<T>;

template <UnsignedInteger T>
class CDynamicBitSet
{
public:
	/**
	 * Empty constructor
	 */
	CDynamicBitSet() : data(nullptr), size(0), storage_size(0) {}

    /**
     * Size and initializer list constructor
     * @param size Size of the CBitSet to be created (in bits)
     * @param list Initializer list to fill the CBitSet with, must contain *chunks* not bits
	 */
    CDynamicBitSet(const uint64_t& size, const std::initializer_list<T> list) : data(new T[calculate_storage_size(size)]), size(size), storage_size(calculate_storage_size(size))
    {
        std::copy(list.begin(), list.end(), data);
    }

    /**
     * Initializer list constructor
     * @param list Initializer list to fill the CBitSet with, must contain *chunks* not bits
     */
    CDynamicBitSet(const std::initializer_list<T> list) : data(new T[list.size() / chunk_size + (list.size() % chunk_size ? 1 : 0)]), size(list.size()), storage_size(list.size() / chunk_size + (list.size() % chunk_size ? 1 : 0))
    {
        std::copy(list.begin(), list.end(), data);
    }

    /**
     * Size constructor
     * @param size Size of the CBitSet to be created (bit count)
     */
    CDynamicBitSet(const uint64_t& size) : data(new T[size / chunk_size + (size % chunk_size ? 1 : 0)]), size(size), storage_size(size / chunk_size + (size % chunk_size ? 1 : 0))
    {
        clear_all();
    }

    /**
     * Size and value constructor
     * @param size Size of the CBitSet to be created (bit count)
     * @param value Value to fill the CBitSet with (bit value)
     */
    CDynamicBitSet(const uint64_t& size, const bool& value) : data(new T[size / chunk_size + (size % chunk_size ? 1 : 0)]), size(size), storage_size(size / chunk_size + (size % chunk_size ? 1 : 0))
    {
        fill_all(value);
    }

    /**
     * Copy constructor
     * @param other Other CDynamicBitSet instance to copy from
     */
    CDynamicBitSet(const CDynamicBitSet& other) : data(new T[other.size / chunk_size + (other.size % chunk_size ? 1 : 0)]), size(other.size), storage_size(other.size / chunk_size + (other.size % chunk_size ? 1 : 0))
    {
        std::copy(other.data, other.data + other.size / chunk_size + (other.size % chunk_size ? 1 : 0), data);
    }

    /**
     * Move constructor
     * @param other Other CDynamicBitSet instance to move from
     */
    CDynamicBitSet(CDynamicBitSet&& other) noexcept : data(other.data), size(other.size), storage_size(other.storage_size)
    {
        other.size = 0;
        other.storage_size = 0;
        other.data = nullptr;
    }

    /**
	 * Destructor
	 */
    ~CDynamicBitSet() { delete[] data; }

    /**
     * Returns the value of the bit at the specified index
     * @param index Index of the bit to retrieve (bit index)
     * @return Value of the bit at the specified index (bit value)
     */
    [[nodiscard]] bool operator[](const uint64_t& index) const
    {
        return get(index);
    }

    /**
     * Copy assignment operator
     * @param other Other CDynamicBitSet instance to copy from
     * @return Reference to the current instance (self)
     */
    CDynamicBitSet& operator=(const CDynamicBitSet& other)
    {
	    if (this != &other)
	    {
            if (size != other.size)
            {
	            delete[] data;
				data = new T[other.size / chunk_size + (other.size % chunk_size ? 1 : 0)];
				size = other.size;
				storage_size = other.size / chunk_size + (other.size % chunk_size ? 1 : 0);
			}
			std::copy(other.data, other.data + other.size / chunk_size + (other.size % chunk_size ? 1 : 0), data);
		}
		return *this;
	}

    /**
     * Move assignment operator
     * @param other Other CDynamicBitSet instance to move from
     * @return Reference to the current instance (self)
     */
    CDynamicBitSet& operator=(CDynamicBitSet&& other) noexcept
    {
	    if (this != &other)
	    {
	    	delete[] data;
			data = other.data;
			size = other.size;
			storage_size = other.storage_size;
            other.size = 0;
            other.storage_size = 0;
            other.data = nullptr;
		}
        return *this;
	}

    /**
     * Sets the bit at the specified index to the specified value
     * @param value Value to set the bit to (bit value)
     * @param index Index of the bit to set (bit index)
     */
    void set(const bool& value, const uint64_t& index)
    {
        if (value)
            *(data + index / chunk_size) |= static_cast<T>(1) << index % chunk_size;
        else
            *(data + index / chunk_size) &= ~(static_cast<T>(1) << index % chunk_size);
    }

    /**
	 * Sets the bit at the specified index to 1 (true)
	 * @param index Index of the bit to set (bit index)
	 */
    void set(const uint64_t& index)
    {
        *(data + index / chunk_size) |= static_cast<T>(1) << index % chunk_size;
    }

    /**
     * Sets the bit at the specified index to 0 (false)
     * @param index Index of the bit to clear (bit index)
     */
    void clear(const uint64_t& index)
    {
        *(data + index / chunk_size) &= ~(static_cast<T>(1) << index % chunk_size);
    }

    /**
     * Fills all the bits with the specified value
     * @param value Value to fill the bits with (bit value)
     */
    void fill_all(const bool& value)
    {
        std::memset(data, value ? (std::numeric_limits<T>::max)() : 0, storage_size * sizeof(T));
    }

    /**
     * Clears all the bits (sets all bits to 0)
     */
    void clear_all()
    {
        std::memset(data, 0, storage_size * sizeof(T));
    }

    /**
     * Fills all the bits with 1 (true)
     */
    void set_all()
    {
        std::memset(data, (std::numeric_limits<T>::max)(), storage_size * sizeof(T));
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const bool& value, const uint64_t& end)
    {
        std::memset(data, value ? (std::numeric_limits<T>::max)() : 0, end / chunk_size * sizeof(T));
        if (end % chunk_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % chunk_size; ++i)
					*(data + end / chunk_size) |= static_cast<T>(1) << i;
            }
            else if (!value)
            {
                for (uint16_t i = 0; i < end % chunk_size; ++i)
                    *(data + end / chunk_size) &= ~(static_cast<T>(1) << i);
            }
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param end End of the range to fill (bit index)
     */
    void clear_in_range(const uint64_t& end)
    {
        std::memset(data, 0, end / chunk_size * sizeof(T));
        if (end % chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                *(data + end / chunk_size) &= ~(static_cast<T>(1) << i);
        }
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param end End of the range to fill (bit index)
     */
    void set_in_range(const uint64_t& end)
	{
		std::memset(data, (std::numeric_limits<T>::max)(), end / chunk_size * sizeof(T));
		if (end % chunk_size)
		{
			for (uint16_t i = 0; i < end % chunk_size; ++i)
				*(data + end / chunk_size) |= static_cast<T>(1) << i;
		}
	}

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end)
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % chunk_size)
        {
            const uint16_t end_bit = (begin / chunk_size == end / chunk_size) ? end % chunk_size : chunk_size;
            if (value)
            {
                for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                    *(data + begin / chunk_size) |= static_cast<T>(1) << i;
            }
            else
            {
                for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                    *(data + begin / chunk_size) &= ~(static_cast<T>(1) << i);
            }
        }
		else
			to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % chunk_size && begin / chunk_size != end / chunk_size)
        {
	        if (value)
	        {
	        	for (uint16_t i = 0; i < end % chunk_size; ++i)
					*(data + end / chunk_size) |= static_cast<T>(1) << i;
			}
			else
			{
				for (uint16_t i = 0; i < end % chunk_size; ++i)
					*(data + end / chunk_size) &= ~(static_cast<T>(1) << i);
			}
		}
        else
            to_sub = 0;

        std::memset(data + begin / chunk_size + to_add, value ? max_value : 0, (end - begin) / chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void clear_in_range(const uint64_t& begin, const uint64_t& end)
    {
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % chunk_size)
        {
            const uint16_t end_bit = begin / chunk_size == end / chunk_size ? end % chunk_size : chunk_size;
            for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                *(data + begin / chunk_size) &= ~(static_cast<T>(1) << i);
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % chunk_size && begin / chunk_size != end / chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                *(data + end / chunk_size) &= ~(static_cast<T>(1) << i);
        }
        else
            to_sub = 0;

        std::memset(data + begin / chunk_size + to_add, 0, (end - begin) / chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void set_in_range(const uint64_t& begin, const uint64_t& end)
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % chunk_size)
        {
            const uint16_t end_bit = (begin / chunk_size == end / chunk_size) ? end % chunk_size : chunk_size;
            for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                *(data + begin / chunk_size) |= static_cast<T>(1) << i;
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % chunk_size && begin / chunk_size != end / chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                *(data + end / chunk_size) |= static_cast<T>(1) << i;
        }
        else
            to_sub = 0;

        std::memset(data + begin / chunk_size + to_add, max_value, (end - begin) / chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            if (value)
                *(data + i / chunk_size) |= static_cast<T>(1) << i % chunk_size;
            else
                *(data + i / chunk_size) &= ~(static_cast<T>(1) << i % chunk_size);
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void clear_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            *(data + i / chunk_size) &= ~(static_cast<T>(1) << i % chunk_size);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void set_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            *(data + i / chunk_size) |= static_cast<T>(1) << i % chunk_size;
    }

    /**
     * !!! W.I.P. - Does not function correctly at the moment !!!\n
     * Fill the bits in the specified range with the specified value using an optimized algorithm.\n
     * This algorithm is particularly efficient when the step size is relatively low.\n
     * Note: This function has a rather complex implementation. It is not recommended to use it when simple filling without a step is possible.\n
     * Performance of this function varies significantly depending on the step. It performs best when step is a multiple of chunk_size, and is within reasonable range from it.\n
     * However, worst when step is not aligned with chunk_size and end is not aligned with chunk_size. In such cases, extra processing is required to handle the boundary chunks.\n
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range_optimized(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        // Initialize variables
        uint64_t chunks_size, current_chunk = begin / chunk_size + 1 + step / chunk_size, current_offset = 0;
        uint16_t offset;
        const uint64_t end_chunk = end / chunk_size + (end % chunk_size ? 1 : 0);

        // Determine the size of chunks based on step and chunk size
        if ((step % 2 || step <= chunk_size) && chunk_size % step) {
            chunks_size = (std::min)(storage_size, step);
        }
        else if (!(chunk_size % step))
            chunks_size = 1;
        else if (!(step % chunk_size))
            chunks_size = step / chunk_size;
        else
        {
            // GCD of step and chunk_size
            if (step % chunk_size)
            {
                uint64_t a = step, b = chunk_size, t = chunk_size;
                while (b) {
                    t = b;
                    b = a % b;
                    a = t;
                }
                chunks_size = a;
            }
            else
                chunks_size = 1;
        }

        // Calculate the offset
        if (begin < chunk_size)
        {
            offset = (chunk_size - begin) % step;
            if (offset)
                offset = step - offset;
        }
        else
        {
            offset = (begin - chunk_size) % step;
        }

        if (offset)
			offset = ((chunk_size - offset) + step / chunk_size * chunk_size) % step;

        std::cout << "offset: " << offset << '\n';

        // Create and apply the beginning chunk
		{
			const uint16_t end_bit = (begin / chunk_size == end / chunk_size) ? end % chunk_size : chunk_size;
        	if (value)
        	{
        		for (uint16_t i = begin % chunk_size; i < end_bit; i += step)
        			*(data + begin / chunk_size) |= static_cast<T>(1) << i;
        	}
        	else
        	{
        		for (uint16_t i = begin % chunk_size; i < end_bit; i += step)
        			*(data + begin / chunk_size) &= ~(static_cast<T>(1) << i);
        	}
		}


        // Fill with appropriate chunk
        std::cout << chunks_size << " chunks\n";
        std::cout << (std::min)(chunks_size + begin / chunk_size, storage_size) << " chunks\n";
        for (uint64_t i = 0; i < (std::min)(chunks_size, storage_size); ++i)
        {
            // Generate chunk for the current iteration
            T chunk = 0;

            if (value)
            {
                for (uint16_t j = !i ? offset : 0; j < chunk_size; ++j)
                {
                    std::cout << current_chunk * chunk_size + j - offset << '\n';
                    if (!((current_chunk * chunk_size + j - offset) % step))
                        chunk |= static_cast<T>(1) << j;
                }
            }
            else
            {
                chunk = (std::numeric_limits<T>::max)();
                for (uint16_t j = (!i ? offset : 0); j < chunk_size; ++j)
                {
                    if (!((current_chunk * chunk_size + j - offset) % step))
                        chunk &= ~(static_cast<T>(1) << j);
                }
            }

            // print the chunk
            for (uint16_t j = 0; j < chunk_size; ++j)
            {
                std::cout << ((chunk & static_cast<T>(1) << j) >> j);
            }

            std::cout << '\n';

            // Apply the chunk
            for (uint64_t j = current_chunk; j < storage_size; ++j)
            {
	            if (j == end_chunk - 1 && end % chunk_size)
	            {
	            	// Remove bits that overflow the range
					if (value)
					{
						for (uint16_t k = end % chunk_size; k < chunk_size; ++k)
							chunk &= ~(static_cast<T>(1) << k);
						*(data + j) |= chunk;
					}
					else
					{
						for (uint16_t k = end % chunk_size; k < chunk_size; ++k)
							chunk |= static_cast<T>(1) << k;
						*(data + j) &= chunk;
					}
					break;
				}
				if (value)
					*(data + j) |= chunk;
				else
					*(data + j) &= chunk;
			}
            ++current_chunk;
        }
    }

    /**
	 * !!! W.I.P. - May not choose the best option, not even talking about the fact that set_in_range_optimized function doesn't even work correctly !!!\n
	 * Fill the bits in the specified range with the specified value.\n
     * Chooses the fastest implementation based on the step.\n
     * This function becomes more accurate in choosing the fastest implementation as the size of the CBitSet increases.\n
     * @param value Value to fill the bits with
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill
    */
    void set_in_range_fastest(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        if (step == 1)
        {
            set_in_range(value, begin, end);
            return;
        }
        if (step <= chunk_size * 2.5) // approximately up until this point it is faster, though no scientific anything went into this, just a guess lol
        {
            fill_in_range_optimized(value, begin, end, step);
            return;
        }
        set_in_range(value, begin, end, step);
    }

    /**
     * Sets the chunk at the specified index to the specified value
     * @param chunk Chunk to set (chunk value)
     * @param index Index of the chunk to set (chunk index)
     */
    void set_chunk(const T& chunk, const uint64_t& index)
    {
        *(data + index) = chunk;
    }

    /**
     * Fills all the chunks with the specified chunk
     * @param chunk Chunk to fill the chunks with (chunk value)
     */
    void fill_chunk(const T& chunk)
    {
        for (uint64_t i = 0; i < storage_size; ++i)
            *(data + i) = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param end End of the range to fill (chunk index)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& end)
    {
        for (uint64_t i = 0; i < end; ++i)
			*(data + i) = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end)
    {
        for (uint64_t i = begin; i < end; ++i)
            *(data + i) = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to fill (chunk step)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            *(data + i) = chunk;
    }

    /**
     * Flips the bit at the specified index
     * @param index Index of the bit to flip (bit index)
     */
    void flip(const uint64_t& index)
    {
	    *(data + index / chunk_size) ^= static_cast<T>(1) << index % chunk_size;
	}

    /**
     * Flips all the bits
     */
    void flip_all()
	{
        for (uint64_t i = 0; i < storage_size; ++i)
            *(data + i) = ~*(data + i);
	}

    /**
     * Flips all the bits in the specified range
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& end)
    {
		// flip chunks that are in range by bulk, rest flip normally
		for (uint64_t i = 0; i < end / chunk_size; ++i)
			*(data + i) = ~*(data + i);
        for (uint16_t i = 0; i < end % chunk_size; ++i)
			*(data + end / chunk_size) ^= static_cast<T>(1) << i;
    }

    /**
     * Flip all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end)
    {
        uint64_t to_add = 1;
        if (begin % chunk_size)
        {
            for (uint16_t i = begin % chunk_size; i < chunk_size; ++i)
                *(data + begin / chunk_size) ^= static_cast<T>(1) << i;
        }
		else
			to_add = 0;

		for (uint64_t i = begin / chunk_size + to_add; i < end / chunk_size; ++i)
			*(data + i) = ~*(data + i);

        if (end % chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
				*(data + end / chunk_size) ^= static_cast<T>(1) << i;
        }
    }

    /**
     * Flips all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to flip (bit step)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            *(data + i / chunk_size) ^= static_cast<T>(1) << i % chunk_size;
        }
    }

    /**
     * Flips the chunk at the specified index
     * @param index Index of the chunk to flip (chunk index)
     */
    void flip_chunk(const uint64_t& index)
    {
	    *(data + index) = ~*(data + index);
    }

    // Flips all the chunks (same as flip_all, just an alias)
    void flip_chunk_all()
    {
        flip_all();
    }

    /**
     * Flips all the chunks in the specified range
     * @param end End of the range to fill (chunk index)
     */
    void flip_chunk_in_range(const uint64_t& end)
    {
        for (uint64_t i = 0; i < end; ++i)
            *(data + i) = ~*(data + i);
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end)
    {
        for (uint64_t i = begin; i < end; ++i)
            *(data + i) = ~*(data + i);
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to flip (chunk step)
     */
    void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            *(data + i) = ~*(data + i);
    }

    /**
     * Retrieves the value of a bit at a specified index
     * @param index The index of the bit to read (bit index)
     * @return The value of the bit at the specified index
     */
    [[nodiscard]] bool get(const uint64_t& index) const
    {
        return (*(data + index / chunk_size) & static_cast<T>(1) << index % chunk_size) >> index % chunk_size;
    }

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] const T& get_chunk(const uint64_t& index) const
    {
	    return *(data + index);
	}

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] T& get_chunk(const uint64_t& index)
    {
        return *(data + index);
    }


    /**
     * Checks if all bits are set 
     * @return true if all bits are set, false otherwise
     */
    [[nodiscard]] bool all() const
    {
        // check all except the last one if the size is not divisible by chunk_size
        for (T* i = data; i < data + storage_size - (size % chunk_size ? 1 : 0); ++i)
        {
            if (*i != (std::numeric_limits<T>::max)())
                return false;
        }
        if (size % chunk_size)
        {
            for (uint16_t i = 0; i < size % chunk_size; ++i)
            {
                if (!(*(data + size / chunk_size) & static_cast<T>(1) << i))
                    return false;
            }
        }
        return true;
    }

    /**
     * Checks if any bit is set
     * @return true if any bit is set, false otherwise
     */
    [[nodiscard]] bool any() const
    {
        for (T* i = data; i < data + storage_size - (size % chunk_size ? 1 : 0); ++i)
        {
            if (*i)
                return true;
        }
        if (size % chunk_size)
		{
			for (uint16_t i = 0; i < size % chunk_size; ++i)
			{
				if (*(data + size / chunk_size) & static_cast<T>(1) << i)
					return true;
			}
		}
        return false;
    }

    /**
     * Checks if none of the bits are set
     * @return true if none of the bits are set, false otherwise
     */
    [[nodiscard]] bool none() const
    {
        for (T* i = data; i < data + storage_size; ++i)
        {
            if (*i)
                return false;
        }
        return true;
    }

    /**
     * Checks if all bits are cleared (none are set)
     * @return true if all bits are cleared, false otherwise
     */
    [[nodiscard]] bool all_clear() const
    {
        return none();
    }

    /**
     * @return The number of set bits
     */
    [[nodiscard]] uint64_t count() const
    {
        uint64_t count = 0;
        for (T* i = data; i < data + storage_size; ++i)
        {
            T j = *i;
            while (j)
            {
                j &= j - 1;
                ++count;
            }
        }
        return count;
    }

    /**
     * Checks if the CBitSet is empty
     * @return true if the CBitSet is empty, false otherwise
     */
    [[nodiscard]] bool empty() const
	{
        return !size;
	}

    /**
     * Pushes back a bit to the CBitSet
     * @param value Value of the bit to append (bit value)
     */
    void push_back(const bool& value)
	{
		if (size % chunk_size)
		{
			if (value)
				*(data + size / chunk_size) |= static_cast<T>(1) << size % chunk_size;
			else
				*(data + size / chunk_size) &= ~(static_cast<T>(1) << size % chunk_size);
		}
		else
		{
			T* new_data = new T[storage_size + 1];
            if (data)
            {
                std::copy(data, data + storage_size, new_data);
                delete[] data;
            }
			data = new_data;
			*(data + storage_size++) = value;
		}
        ++size;
	}

    /**
     * Removes the last bit from the CBitSet
     */
    void pop_back()
    {
        if (data)
        {
            if (!(size % chunk_size))
            {
                T* new_data = new T[storage_size - 1];
                std::copy(data, data + storage_size, new_data);
                delete[] data;
                data = new_data;
            }
            --size;
        }
        // else throw exception in safe version
    }

    /**
     * Pushes back a chunk to the CBitSet, adjusting the size to the nearest multiple of sizeof(T) upwards. [e.g. 65 bits -> (+8 {chunk} +7 {expanded area} = +15) -> 80 bits]
     * The bits in the expanded area may be initialized by previous calls, but their values are not explicitly defined by this function.
     * @param chunk The chunk to push back (chunk value)
     */
    void push_back_chunk(const T& chunk)
    {
        T* new_data = new T[storage_size + (size % chunk_size ? 2 : 1)];
        if (data)
        {
            std::copy(data, data + storage_size, new_data);
            delete[] data;
        }
        data = new_data;
        *(data + storage_size++) = chunk;
        size += chunk_size + (size % chunk_size ? chunk_size - size % chunk_size : 0);
    }

    /**
     * Removes the last chunk from the CBitSet, adjusting the size to the nearest lower multiple of sizeof(T). [e.g. 65 bits -> 64 bits -> 56 bits]
     */
    void pop_back_chunk()
    {
        if (data)
        {
            if (size % chunk_size)
                size = (storage_size - 1) * chunk_size;
            
            T* new_data = new T[storage_size - 1];
            std::copy(data, data + storage_size - 1, new_data);
            delete[] data;
            data = new_data;
            --storage_size;
            size -= chunk_size;
        }
        // else throw error in safe version
    }

	/**
	 * Resizes the CBitSet to the specified size
	 * @param new_size The new size of the CBitSet (bit size)
	 */
	void resize(const uint64_t& new_size)
	{
		if (new_size == size)
			return;
		if (new_size < size)
		{
			size = new_size;
			storage_size = calculate_storage_size(size);
			return;
		}
		const uint64_t new_storage_size = calculate_storage_size(new_size);
		T* new_data = new T[new_storage_size];
		if (data)
		{
			std::copy(data, data + storage_size, new_data);
			delete[] data;
		}
		data = new_data;
		storage_size = new_storage_size;
		size = new_size;
	}

    /**
     * Returns the number of chunks the CBitSet would utilize for given size
     * @param size Size of the target CBitSet in bits
     * @return The number of chunks the CBitSet would utilize for the given size
     */
    [[nodiscard]] inline static constexpr uint64_t calculate_storage_size(const uint64_t& size)
    {
        return size / (sizeof(T) * 8) + (size % (sizeof(T) * 8) ? 1 : 0);
    }

	/**
	 * Underlying array of chunks containing the bits
	 */
	alignas(std::hardware_destructive_interference_size) T* data;

	/**
	 * Size of the CBitSet in bits
	 */
	uint64_t size;

	/**
	 * Amount of chunks the CBitSet is utilizing
	 */
	uint64_t storage_size;

	/**
	 * Bit-length of the underlying type
	 */
	static constexpr uint16_t chunk_size = sizeof(T) * 8;
};

template <UnsignedInteger T, uint64_t Size>
class CBitSet
{
public:
    /**
     * Empty constructor
     */
    CBitSet() = default;

    /**
     * Initializer list constructor
     * @param list Initializer list to fill the CBitSet with, must contain *chunks* not bits
     */
    CBitSet(const std::initializer_list<T> list)
    {
        std::copy(list.begin(), list.end(), data);
    }

    /**
     * Value constructor
     * @param value Value to fill the CBitSet with (bit value)
     */
    CBitSet(const bool& value)
    {
        fill_all(value);
    }

    /**
     * Copy constructor
     * @param other Other CDynamicBitSet instance to copy from
     */
    CBitSet(const CBitSet& other)
    {
        std::copy(other.data, other.data + other.size / chunk_size + (other.size % chunk_size ? 1 : 0), data);
    }

    /**
     * Destructor
     */
    ~CBitSet() = default;

    /**
     * Returns the value of the bit at the specified index
     * @param index Index of the bit to retrieve (bit index)
     * @return Value of the bit at the specified index (bit value)
     */
    [[nodiscard]] bool operator[](const uint64_t& index) const
    {
        return get(index);
    }

    /**
     * Copy assignment operator
     * @param other Other CDynamicBitSet instance to copy from
     * @return Reference to the current instance (self)
     */
    CBitSet& operator=(const CBitSet& other)
    {
        if (this != &other)
            std::copy(other.data, other.data + storage_size, data);
        
        return *this;
    }

    /**
     * Sets the bit at the specified index to the specified value
     * @param value Value to set the bit to (bit value)
     * @param index Index of the bit to set (bit index)
     */
    void set(const bool& value, const uint64_t& index)
    {
        if (value)
            data[index / chunk_size] |= static_cast<T>(1) << index % chunk_size;
        else
            data[index / chunk_size] &= ~(static_cast<T>(1) << index % chunk_size);
    }

    /**
     * Sets the bit at the specified index to 1 (true)
     * @param index Index of the bit to set (bit index)
     */
    void set(const uint64_t& index)
    {
        data[index / chunk_size] |= static_cast<T>(1) << index % chunk_size;
    }

    /**
     * Sets the bit at the specified index to 0 (false)
     * @param index Index of the bit to clear (bit index)
     */
    void clear(const uint64_t& index)
    {
        data[index / chunk_size] &= ~(static_cast<T>(1) << index % chunk_size);
    }

    /**
     * Fills all the bits with the specified value
     * @param value Value to fill the bits with (bit value)
     */
    void fill_all(const bool& value)
    {
        std::memset(data, value ? (std::numeric_limits<T>::max)() : 0, storage_size * sizeof(T));
    }

    /**
     * Clears all the bits (sets all bits to 0)
     */
    void clear_all()
    {
        std::memset(data, 0, storage_size * sizeof(T));
    }

    /**
     * Fills all the bits with 1 (true)
     */
    void set_all()
    {
        std::memset(data, (std::numeric_limits<T>::max)(), storage_size * sizeof(T));
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const bool& value, const uint64_t& end)
    {
        std::memset(data, value ? (std::numeric_limits<T>::max)() : 0, end / chunk_size * sizeof(T));
        if (end % chunk_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % chunk_size; ++i)
                    data[end / chunk_size] |= static_cast<T>(1) << i;
            }
            else if (!value)
            {
                for (uint16_t i = 0; i < end % chunk_size; ++i)
                    data[end / chunk_size] &= ~(static_cast<T>(1) << i);
            }
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param end End of the range to fill (bit index)
     */
    void clear_in_range(const uint64_t& end)
    {
        std::memset(data, 0, end / chunk_size * sizeof(T));
        if (end % chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                data[end / chunk_size] &= ~(static_cast<T>(1) << i);
        }
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param end End of the range to fill (bit index)
     */
    void set_in_range(const uint64_t& end)
    {
        std::memset(data, (std::numeric_limits<T>::max)(), end / chunk_size * sizeof(T));
        if (end % chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                data[end / chunk_size] |= static_cast<T>(1) << i;
        }
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end)
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % chunk_size)
        {
            const uint16_t end_bit = (begin / chunk_size == end / chunk_size) ? end % chunk_size : chunk_size;
            if (value)
            {
                for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                    data[begin / chunk_size] |= static_cast<T>(1) << i;
            }
            else
            {
                for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                    data[begin / chunk_size] &= ~(static_cast<T>(1) << i);
            }
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % chunk_size && begin / chunk_size != end / chunk_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % chunk_size; ++i)
                    data[end / chunk_size] |= static_cast<T>(1) << i;
            }
            else
            {
                for (uint16_t i = 0; i < end % chunk_size; ++i)
                    data[end / chunk_size] &= ~(static_cast<T>(1) << i);
            }
        }
        else
            to_sub = 0;

        std::memset(data + begin / chunk_size + to_add, value ? max_value : 0, (end - begin) / chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void clear_in_range(const uint64_t& begin, const uint64_t& end)
    {
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % chunk_size)
        {
            const uint16_t end_bit = begin / chunk_size == end / chunk_size ? end % chunk_size : chunk_size;
            for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                data[end / chunk_size] &= ~(static_cast<T>(1) << i);
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % chunk_size && begin / chunk_size != end / chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                data[end / chunk_size] &= ~(static_cast<T>(1) << i);
        }
        else
            to_sub = 0;

        std::memset(data + begin / chunk_size + to_add, 0, (end - begin) / chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void set_in_range(const uint64_t& begin, const uint64_t& end)
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % chunk_size)
        {
            const uint16_t end_bit = (begin / chunk_size == end / chunk_size) ? end % chunk_size : chunk_size;
            for (uint16_t i = begin % chunk_size; i < end_bit; ++i)
                data[begin / chunk_size] |= static_cast<T>(1) << i;
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % chunk_size && begin / chunk_size != end / chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                data[end / chunk_size] |= static_cast<T>(1) << i;
        }
        else
            to_sub = 0;

        std::memset(data + begin / chunk_size + to_add, max_value, (end - begin) / chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            if (value)
                data[i / chunk_size] |= static_cast<T>(1) << i % chunk_size;
            else
                data[i / chunk_size] &= ~(static_cast<T>(1) << i % chunk_size);
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void clear_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            data[i / chunk_size] &= ~(static_cast<T>(1) << i % chunk_size);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void set_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            data[i / chunk_size] |= static_cast<T>(1) << i % chunk_size;
    }

    /**
     * !!! W.I.P. - Does not function correctly at the moment !!!\n
     * Fill the bits in the specified range with the specified value using an optimized algorithm.\n
     * This algorithm is particularly efficient when the step size is relatively low.\n
     * Note: This function has a rather complex implementation. It is not recommended to use it when simple filling without a step is possible.\n
     * Performance of this function varies significantly depending on the step. It performs best when step is a multiple of chunk_size, and is within reasonable range from it.\n
     * However, worst when step is not aligned with chunk_size and end is not aligned with chunk_size. In such cases, extra processing is required to handle the boundary chunks.\n
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range_optimized(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        // Initialize variables
        uint64_t chunks_size, current_chunk = begin / chunk_size + 1 + step / chunk_size, current_offset = 0;
        uint16_t offset;
        const uint64_t end_chunk = end / chunk_size + (end % chunk_size ? 1 : 0);

        // Determine the size of chunks based on step and chunk size
        if ((step % 2 || step <= chunk_size) && chunk_size % step) {
            chunks_size = (std::min)(storage_size, step);
        }
        else if (!(chunk_size % step))
            chunks_size = 1;
        else if (!(step % chunk_size))
            chunks_size = step / chunk_size;
        else
        {
            // GCD of step and chunk_size
            if (step % chunk_size)
            {
                uint64_t a = step, b = chunk_size, t = chunk_size;
                while (b) {
                    t = b;
                    b = a % b;
                    a = t;
                }
                chunks_size = a;
            }
            else
                chunks_size = 1;
        }

        // Calculate the offset
        if (begin < chunk_size)
        {
            offset = (chunk_size - begin) % step;
            if (offset)
                offset = step - offset;
        }
        else
        {
            offset = (begin - chunk_size) % step;
        }

        if (offset)
            offset = ((chunk_size - offset) + step / chunk_size * chunk_size) % step;

        std::cout << "offset: " << offset << '\n';

        // Create and apply the beginning chunk
        {
            const uint16_t end_bit = (begin / chunk_size == end / chunk_size) ? end % chunk_size : chunk_size;
            if (value)
            {
                for (uint16_t i = begin % chunk_size; i < end_bit; i += step)
                    *(data + begin / chunk_size) |= static_cast<T>(1) << i;
            }
            else
            {
                for (uint16_t i = begin % chunk_size; i < end_bit; i += step)
                    *(data + begin / chunk_size) &= ~(static_cast<T>(1) << i);
            }
        }


        // Fill with appropriate chunk
        std::cout << chunks_size << " chunks\n";
        std::cout << (std::min)(chunks_size + begin / chunk_size, storage_size) << " chunks\n";
        for (uint64_t i = 0; i < (std::min)(chunks_size, storage_size); ++i)
        {
            // Generate chunk for the current iteration
            T chunk = 0;

            if (value)
            {
                for (uint16_t j = !i ? offset : 0; j < chunk_size; ++j)
                {
                    std::cout << current_chunk * chunk_size + j - offset << '\n';
                    if (!((current_chunk * chunk_size + j - offset) % step))
                        chunk |= static_cast<T>(1) << j;
                }
            }
            else
            {
                chunk = (std::numeric_limits<T>::max)();
                for (uint16_t j = (!i ? offset : 0); j < chunk_size; ++j)
                {
                    if (!((current_chunk * chunk_size + j - offset) % step))
                        chunk &= ~(static_cast<T>(1) << j);
                }
            }

            // print the chunk
            for (uint16_t j = 0; j < chunk_size; ++j)
            {
                std::cout << ((chunk & static_cast<T>(1) << j) >> j);
            }

            std::cout << '\n';

            // Apply the chunk
            for (uint64_t j = current_chunk; j < storage_size; ++j)
            {
                if (j == end_chunk - 1 && end % chunk_size)
                {
                    // Remove bits that overflow the range
                    if (value)
                    {
                        for (uint16_t k = end % chunk_size; k < chunk_size; ++k)
                            chunk &= ~(static_cast<T>(1) << k);
                        *(data + j) |= chunk;
                    }
                    else
                    {
                        for (uint16_t k = end % chunk_size; k < chunk_size; ++k)
                            chunk |= static_cast<T>(1) << k;
                        *(data + j) &= chunk;
                    }
                    break;
                }
                if (value)
                    *(data + j) |= chunk;
                else
                    *(data + j) &= chunk;
            }
            ++current_chunk;
        }
    }

    /**
     * !!! W.I.P. - May not choose the best option, not even talking about the fact that set_in_range_optimized function doesn't even work correctly !!!\n
     * Fill the bits in the specified range with the specified value.\n
     * Chooses the fastest implementation based on the step.\n
     * This function becomes more accurate in choosing the fastest implementation as the size of the CBitSet increases.\n
     * @param value Value to fill the bits with
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill
    */
    void set_in_range_fastest(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        if (step == 1)
        {
            set_in_range(value, begin, end);
            return;
        }
        if (step <= chunk_size * 2.5) // approximately up until this point it is faster, though no scientific anything went into this, just a guess lol
        {
            fill_in_range_optimized(value, begin, end, step);
            return;
        }
        set_in_range(value, begin, end, step);
    }

    /**
     * Sets the chunk at the specified index to the specified value
     * @param chunk Chunk to set (chunk value)
     * @param index Index of the chunk to set (chunk index)
     */
    void set_chunk(const T& chunk, const uint64_t& index)
    {
        data[index] = chunk;
    }

    /**
     * Fills all the chunks with the specified chunk
     * @param chunk Chunk to fill the chunks with (chunk value)
     */
    void fill_chunk(const T& chunk)
    {
        for (uint64_t i = 0; i < storage_size; ++i)
            data[i] = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param end End of the range to fill (chunk index)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& end)
    {
        for (uint64_t i = 0; i < end; ++i)
            data[i] = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end)
    {
        for (uint64_t i = begin; i < end; ++i)
            data[i] = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to fill (chunk step)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            data[i] = chunk;
    }

    /**
     * Flips the bit at the specified index
     * @param index Index of the bit to flip (bit index)
     */
    void flip(const uint64_t& index)
    {
    	data[index / chunk_size] ^= static_cast<T>(1) << index % chunk_size;
    }

    /**
     * Flips all the bits
     */
    void flip_all()
    {
        for (uint64_t i = 0; i < storage_size; ++i)
            data[i] = ~data[i];
    }

    /**
     * Flips all the bits in the specified range
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& end)
    {
        // flip chunks that are in range by bulk, rest flip normally
        for (uint64_t i = 0; i < end / chunk_size; ++i)
            data[i] = ~data[i];
        for (uint16_t i = 0; i < end % chunk_size; ++i)
            data[end / chunk_size] ^= static_cast<T>(1) << i;
    }

    /**
     * Flip all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end)
    {
        uint64_t to_add = 1;
        if (begin % chunk_size)
        {
            for (uint16_t i = begin % chunk_size; i < chunk_size; ++i)
                data[begin / chunk_size] ^= static_cast<T>(1) << i;
        }
        else
            to_add = 0;

        for (uint64_t i = begin / chunk_size + to_add; i < end / chunk_size; ++i)
            data[i] = ~data[i];

        if (end % chunk_size)
        {
            for (uint16_t i = 0; i < end % chunk_size; ++i)
                data[end / chunk_size] ^= static_cast<T>(1) << i;
        }
    }

    /**
     * Flips all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to flip (bit step)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            data[i] ^= static_cast<T>(1) << i % chunk_size;
    }

    /**
     * Flips the chunk at the specified index
     * @param index Index of the chunk to flip (chunk index)
     */
    void flip_chunk(const uint64_t& index)
    {
        data[index] = ~data[index];
    }

    // Flips all the chunks (same as flip_all, just an alias)
    void flip_chunk_all()
    {
        flip_all();
    }

    /**
     * Flips all the chunks in the specified range
     * @param end End of the range to fill (chunk index)
     */
    void flip_chunk_in_range(const uint64_t& end)
    {
        for (uint64_t i = 0; i < end; ++i)
            data[i] = ~data[i];
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end)
    {
        for (uint64_t i = begin; i < end; ++i)
            data[i] = ~data[i];
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to flip (chunk step)
     */
    void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            data[i] = ~data[i];
    }

    /**
     * Retrieves the value of a bit at a specified index
     * @param index The index of the bit to read (bit index)
     * @return The value of the bit at the specified index
     */
    [[nodiscard]] bool get(const uint64_t& index) const
    {
        return (data[index / chunk_size] & static_cast<T>(1) << index % chunk_size) >> index % chunk_size;
    }

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] const T& get_chunk(const uint64_t& index) const
    {
        return data[index];
    }

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] T& get_chunk(const uint64_t& index)
    {
        return data[index];
    }


    /**
     * Checks if all bits are set
     * @return true if all bits are set, false otherwise
     */
    [[nodiscard]] bool all() const
    {
        // check all except the last one if the size is not divisible by chunk_size
        for (T* i = data; i < data + storage_size - (size % chunk_size ? 1 : 0); ++i)
        {
            if (*i != (std::numeric_limits<T>::max)())
                return false;
        }
        if (size % chunk_size)
        {
            for (uint16_t i = 0; i < size % chunk_size; ++i)
            {
                if (!(*(data + size / chunk_size) & static_cast<T>(1) << i))
                    return false;
            }
        }
        return true;
    }

    /**
     * Checks if any bit is set
     * @return true if any bit is set, false otherwise
     */
    [[nodiscard]] bool any() const
    {
        for (T* i = data; i < data + storage_size - (size % chunk_size ? 1 : 0); ++i)
        {
            if (*i)
                return true;
        }
        if (size % chunk_size)
        {
            for (uint16_t i = 0; i < size % chunk_size; ++i)
            {
                if (*(data + size / chunk_size) & static_cast<T>(1) << i)
                    return true;
            }
        }
        return false;
    }

    /**
     * Checks if none of the bits are set
     * @return true if none of the bits are set, false otherwise
     */
    [[nodiscard]] bool none() const
    {
        for (T* i = data; i < data + storage_size; ++i)
        {
            if (*i)
                return false;
        }
        return true;
    }

    /**
     * Checks if all bits are cleared (none are set)
     * @return true if all bits are cleared, false otherwise
     */
    [[nodiscard]] bool all_clear() const
    {
        return none();
    }

    /**
     * @return The number of set bits
     */
    [[nodiscard]] uint64_t count() const
    {
        uint64_t count = 0;
        for (T* i = data; i < data + storage_size; ++i)
        {
            T j = *i;
            while (j)
            {
                j &= j - 1;
                ++count;
            }
        }
        return count;
    }

    /**
     * Checks if the CBitSet is empty
     * @return true if the CBitSet is empty, false otherwise
     */
    [[nodiscard]] static bool empty()
    {
        return !size;
    }

    /**
     * Returns the number of chunks the CBitSet would utilize for given size
     * @param size Size of the target CBitSet in bits
     * @return The number of chunks the CBitSet would utilize for the given size
     */
    [[nodiscard]] inline static constexpr uint64_t calculate_storage_size(const uint64_t& size)
    {
        return size / (sizeof(T) * 8) + (size % (sizeof(T) * 8) ? 1 : 0);
    }

    /**
     * Size of the CBitSet in bits
     */
    static constexpr uint64_t size = Size;

    /**
     * Amount of chunks the CBitSet is utilizing
     */
    static constexpr uint64_t storage_size = calculate_storage_size(size);

    /**
     * Bit-length of the underlying type
     */
    static constexpr uint16_t chunk_size = sizeof(T) * 8;

    /**
	 * Underlying array of chunks containing the bits
	 */
    alignas(std::hardware_destructive_interference_size) T data[storage_size] = { 0 };
};