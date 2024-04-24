#pragma once
#include <algorithm>
#include <cstdint>
#include <limits>
#include <iostream>
#include <concepts>
#include <new>
#include <cstring>

// Note: (std::numeric_limits<T>::max)() is used instead of std::numeric_limits<T>::max() because Windows.h defines a macro max which conflicts with std::numeric_limits<T>::max()

namespace bit_set
{}

template <typename T>
concept UnsignedInteger = std::is_unsigned_v<T> && std::is_integral_v<T>;

template <UnsignedInteger T>
class CDynamicBitSet
{
public:
	/**
	 * Empty constructor
	 */
	CDynamicBitSet() noexcept : m_data(nullptr), m_size(0), m_storage_size(0) {}

    /**
     * Size and initializer list constructor
     * @param size Size of the Bitset to be created (in bits)
     * @param list Initializer list to fill the Bitset with, containing chunk values
	 */
    CDynamicBitSet(const uint64_t& size, const std::initializer_list<T> list) noexcept : m_data(new T[calculate_storage_size(size)]), m_size(size), m_storage_size(calculate_storage_size(size))
    {
        std::copy(list.begin(), list.end(), m_data);
    }

    /**
     * Initializer list constructor
     * @param list Initializer list to fill the Bitset with, containing chunk values
     */
    CDynamicBitSet(const std::initializer_list<T> list) noexcept : m_data(new T[list.m_size() / m_chunk_size + (list.m_size() % m_chunk_size ? 1 : 0)]), m_size(list.m_size()), m_storage_size(list.m_size() / m_chunk_size + (list.m_size() % m_chunk_size ? 1 : 0))
    {
        std::copy(list.begin(), list.end(), m_data);
    }

    /**
     * Size constructor
     * @param size Size of the Bitset to be created (bit count)
     */
    CDynamicBitSet(const uint64_t& size) noexcept : m_data(new T[size / m_chunk_size + (size % m_chunk_size ? 1 : 0)]), m_size(size), m_storage_size(size / m_chunk_size + (size % m_chunk_size ? 1 : 0))
    {
        clear();
    }

    /**
     * Size and chunk value constructor
     * @param size Size of the bitset to be created (bit size)
     * @param chunk Chunk to fill the bitset with (chunk value)
     */
    CDynamicBitSet(const uint64_t& size, const T& chunk) noexcept : m_data(new T[size / m_chunk_size + (size % m_chunk_size ? 1 : 0)]{ chunk }), m_size(size), m_storage_size(size / m_chunk_size + (size % m_chunk_size ? 1 : 0)) {}

    /**
     * Copy constructor
     * @param other Other CDynamicBitSet instance to copy from
     */
    CDynamicBitSet(const CDynamicBitSet& other) noexcept : m_data(new T[other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0)]), m_size(other.m_size), m_storage_size(other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0))
    {
        std::copy(other.m_data, other.m_data + other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0), m_data);
    }

    /**
     * Move constructor
     * @param other Other CDynamicBitSet instance to move from
     */
    CDynamicBitSet(CDynamicBitSet&& other) noexcept : m_data(other.m_data), m_size(other.m_size), m_storage_size(other.m_storage_size)
    {
        other.m_size = 0;
        other.m_storage_size = 0;
        other.m_data = nullptr;
    }

    /**
	 * Destructor
	 */
    ~CDynamicBitSet() { delete[] m_data; }

    /**
     * Returns the value of the bit at the specified index
     * @param index Index of the bit to retrieve (bit index)
     * @return Value of the bit at the specified index (bit value)
     */
    [[nodiscard]] bool operator[](const uint64_t& index) const noexcept
    {
        return get(index);
    }

    /**
     * Copy assignment operator
     * @param other Other CDynamicBitSet instance to copy from
     * @return Reference to the current instance (self)
     */
    CDynamicBitSet& operator=(const CDynamicBitSet& other) noexcept
    {
	    if (this != &other)
	    {
            if (m_size != other.m_size)
            {
	            delete[] m_data;
                m_data = new T[other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0)];
                m_size = other.m_size;
                m_storage_size = other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0);
			}
			std::copy(other.m_data, other.m_data + other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0), m_data);
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
	    	delete[] m_data;
            m_data = other.m_data;
            m_size = other.m_size;
            m_storage_size = other.m_storage_size;
            other.m_size = other.m_storage_size = 0;
            other.m_data = nullptr;
		}
        return *this;
	}

    /**
     * Sets the bit at the specified index to the specified value
     * @param value Value to set the bit to (bit value)
     * @param index Index of the bit to set (bit index)
     */
    void set(const bool& value, const uint64_t& index) noexcept
    {
        if (value)
            *(m_data + index / m_chunk_size) |= T{ 1 } << index % m_chunk_size;
        else
            *(m_data + index / m_chunk_size) &= ~(T{ 1 } << index % m_chunk_size);
    }

    /**
	 * Sets the bit at the specified index to 1 (true)
	 * @param index Index of the bit to set (bit index)
	 */
    void set(const uint64_t& index) noexcept
    {
        *(m_data + index / m_chunk_size) |= T{ 1 } << index % m_chunk_size;
    }

    /**
     * Sets the bit at the specified index to 0 (false)
     * @param index Index of the bit to clear (bit index)
     */
    void clear(const uint64_t& index) noexcept
    {
        *(m_data + index / m_chunk_size) &= ~(T{ 1 } << index % m_chunk_size);
    }

    /**
     * Fills all the bits with the specified value
     * @param value Value to fill the bits with (bit value)
     */
    void fill(const bool& value) noexcept
    {
        ::memset(m_data, value ? (std::numeric_limits<T>::max)() : 0, m_storage_size * sizeof(T));
    }

    /**
     * Clears all the bits (sets all bits to 0)
     */
    void clear() noexcept
    {
        ::memset(m_data, 0, m_storage_size * sizeof(T));
    }

    /**
     * Fills all the bits with 1 (true)
     */
    void set() noexcept
    {
        ::memset(m_data, (std::numeric_limits<T>::max)(), m_storage_size * sizeof(T));
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const bool& value, const uint64_t& end) noexcept
    {
        ::memset(m_data, value ? (std::numeric_limits<T>::max)() : 0, end / m_chunk_size * sizeof(T));
        if (end % m_chunk_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % m_chunk_size; ++i)
					*(m_data + end / m_chunk_size) |= T{ 1 } << i;
            }
            else if (!value)
            {
                for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                    *(m_data + end / m_chunk_size) &= ~(T{ 1 } << i);
            }
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param end End of the range to fill (bit index)
     */
    void clear_in_range(const uint64_t& end) noexcept
    {
        ::memset(m_data, 0, end / m_chunk_size * sizeof(T));
        if (end % m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                *(m_data + end / m_chunk_size) &= ~(T{ 1 } << i);
        }
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param end End of the range to fill (bit index)
     */
    void set_in_range(const uint64_t& end) noexcept
	{
		::memset(m_data, (std::numeric_limits<T>::max)(), end / m_chunk_size * sizeof(T));
		if (end % m_chunk_size)
		{
			for (uint16_t i = 0; i < end % m_chunk_size; ++i)
				*(m_data + end / m_chunk_size) |= T{ 1 } << i;
		}
	}

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end) noexcept
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % m_chunk_size)
        {
            const uint16_t end_bit = (begin / m_chunk_size == end / m_chunk_size) ? end % m_chunk_size : m_chunk_size;
            if (value)
            {
                for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                    *(m_data + begin / m_chunk_size) |= T{ 1 } << i;
            }
            else
            {
                for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                    *(m_data + begin / m_chunk_size) &= ~(T{ 1 } << i);
            }
        }
		else
			to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % m_chunk_size && begin / m_chunk_size != end / m_chunk_size)
        {
	        if (value)
	        {
	        	for (uint16_t i = 0; i < end % m_chunk_size; ++i)
					*(m_data + end / m_chunk_size) |= T{ 1 } << i;
			}
			else
			{
				for (uint16_t i = 0; i < end % m_chunk_size; ++i)
					*(m_data + end / m_chunk_size) &= ~(T{ 1 } << i);
			}
		}
        else
            to_sub = 0;

        ::memset(m_data + begin / m_chunk_size + to_add, value ? max_value : 0, (end - begin) / m_chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void clear_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % m_chunk_size)
        {
            const uint16_t end_bit = begin / m_chunk_size == end / m_chunk_size ? end % m_chunk_size : m_chunk_size;
            for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                *(m_data + begin / m_chunk_size) &= ~(T{ 1 } << i);
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % m_chunk_size && begin / m_chunk_size != end / m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                *(m_data + end / m_chunk_size) &= ~(T{ 1 } << i);
        }
        else
            to_sub = 0;

        ::memset(m_data + begin / m_chunk_size + to_add, 0, (end - begin) / m_chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void set_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % m_chunk_size)
        {
            const uint16_t end_bit = (begin / m_chunk_size == end / m_chunk_size) ? end % m_chunk_size : m_chunk_size;
            for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                *(m_data + begin / m_chunk_size) |= T{ 1 } << i;
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % m_chunk_size && begin / m_chunk_size != end / m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                *(m_data + end / m_chunk_size) |= T{ 1 } << i;
        }
        else
            to_sub = 0;

        ::memset(m_data + begin / m_chunk_size + to_add, max_value, (end - begin) / m_chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            if (value)
                *(m_data + i / m_chunk_size) |= T{ 1 } << i % m_chunk_size;
            else
                *(m_data + i / m_chunk_size) &= ~(T{ 1 } << i % m_chunk_size);
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void clear_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            *(m_data + i / m_chunk_size) &= ~(T{ 1 } << i % m_chunk_size);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void set_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            *(m_data + i / m_chunk_size) |= T{ 1 } << i % m_chunk_size;
    }

    /**
     * !!! W.I.P. - Does not function correctly at the moment !!!\n
     * Fill the bits in the specified range with the specified value using an optimized algorithm.\n
     * This algorithm is particularly efficient when the step size is relatively low.\n
     * Note: This function has a rather complex implementation. It is not recommended to use it when simple filling without a step is possible.\n
     * Performance of this function varies significantly depending on the step. It performs best when step is a multiple of m_chunk_size, and is within reasonable range from it.\n
     * However, worst when step is not aligned with m_chunk_size and end is not aligned with m_chunk_size. In such cases, extra processing is required to handle the boundary chunks.\n
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range_optimized(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        // Initialize variables
        uint64_t chunks_size, current_chunk = begin / m_chunk_size + 1 + step / m_chunk_size, current_offset = 0;
        uint16_t offset;
        const uint64_t end_chunk = end / m_chunk_size + (end % m_chunk_size ? 1 : 0);

        // Determine the size of chunks based on step and chunk size
        if ((step % 2 || step <= m_chunk_size) && m_chunk_size % step) {
            chunks_size = (std::min)(m_storage_size, step);
        }
        else if (!(m_chunk_size % step))
            chunks_size = 1;
        else if (!(step % m_chunk_size))
            chunks_size = step / m_chunk_size;
        else
        {
            // GCD of step and m_chunk_size
            if (step % m_chunk_size)
            {
                uint64_t a = step, b = m_chunk_size, t = m_chunk_size;
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
        if (begin < m_chunk_size)
        {
            offset = (m_chunk_size - begin) % step;
            if (offset)
                offset = step - offset;
        }
        else
        {
            offset = (begin - m_chunk_size) % step;
        }

        if (offset)
			offset = (m_chunk_size - offset + step / m_chunk_size * m_chunk_size) % step;

        std::cout << "offset: " << offset << '\n';

        // Create and apply the beginning chunk
		{
			const uint16_t end_bit = (begin / m_chunk_size == end / m_chunk_size) ? end % m_chunk_size : m_chunk_size;
        	if (value)
        	{
        		for (uint16_t i = begin % m_chunk_size; i < end_bit; i += step)
        			*(m_data + begin / m_chunk_size) |= T{ 1 } << i;
        	}
        	else
        	{
        		for (uint16_t i = begin % m_chunk_size; i < end_bit; i += step)
        			*(m_data + begin / m_chunk_size) &= ~(T{ 1 } << i);
        	}
		}


        // Fill with appropriate chunk
        std::cout << chunks_size << " chunks\n";
        std::cout << (std::min)(chunks_size + begin / m_chunk_size, m_storage_size) << " chunks\n";
        for (uint64_t i = 0; i < (std::min)(chunks_size, m_storage_size); ++i)
        {
            // Generate chunk for the current iteration
            T chunk = 0;

            if (value)
            {
                for (uint16_t j = !i ? offset : 0; j < m_chunk_size; ++j)
                {
                    std::cout << current_chunk * m_chunk_size + j - offset << '\n';
                    if (!((current_chunk * m_chunk_size + j - offset) % step))
                        chunk |= T{ 1 } << j;
                }
            }
            else
            {
                chunk = (std::numeric_limits<T>::max)();
                for (uint16_t j = (!i ? offset : 0); j < m_chunk_size; ++j)
                {
                    if (!((current_chunk * m_chunk_size + j - offset) % step))
                        chunk &= ~(T{ 1 } << j);
                }
            }

            // print the chunk
            for (uint16_t j = 0; j < m_chunk_size; ++j)
            {
                std::cout << ((chunk & T{ 1 } << j) >> j);
            }

            std::cout << '\n';

            // Apply the chunk
            for (uint64_t j = current_chunk; j < m_storage_size; ++j)
            {
	            if (j == end_chunk - 1 && end % m_chunk_size)
	            {
	            	// Remove bits that overflow the range
					if (value)
					{
						for (uint16_t k = end % m_chunk_size; k < m_chunk_size; ++k)
							chunk &= ~(T{ 1 } << k);
						*(m_data + j) |= chunk;
					}
					else
					{
						for (uint16_t k = end % m_chunk_size; k < m_chunk_size; ++k)
							chunk |= T{ 1 } << k;
						*(m_data + j) &= chunk;
					}
					break;
				}
				if (value)
					*(m_data + j) |= chunk;
				else
					*(m_data + j) &= chunk;
			}
            ++current_chunk;
        }
    }

    /**
	 * !!! W.I.P. - May not choose the best option, not even talking about the fact that set_in_range_optimized function doesn't even work correctly !!!\n
	 * Fill the bits in the specified range with the specified value.\n
     * Chooses the fastest implementation based on the step.\n
     * This function becomes more accurate in choosing the fastest implementation as the size of the Bitset increases.\n
     * @param value Value to fill the bits with
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill
    */
    void set_in_range_fastest(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        if (step == 1)
        {
            set_in_range(value, begin, end);
            return;
        }
        if (step <= m_chunk_size * 2.5) // approximately up until this point it is faster, though no scientific anything went into this, just a guess lol
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
    void set_chunk(const T& chunk, const uint64_t& index) noexcept
    {
        *(m_data + index) = chunk;
    }

    /**
     * Fills all the chunks with the specified chunk
     * @param chunk Chunk to fill the chunks with (chunk value)
     */
    void fill_chunk(const T& chunk) noexcept
    {
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(m_data + i) = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param end End of the range to fill (chunk index)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& end) noexcept
    {
        for (uint64_t i = 0; i < end; ++i)
			*(m_data + i) = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end) noexcept
    {
        for (uint64_t i = begin; i < end; ++i)
            *(m_data + i) = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to fill (chunk step)
     */
    void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            *(m_data + i) = chunk;
    }

    /**
     * Flips the bit at the specified index
     * @param index Index of the bit to flip (bit index)
     */
    void flip(const uint64_t& index) noexcept
    {
	    *(m_data + index / m_chunk_size) ^= T{ 1 } << index % m_chunk_size;
	}

    /**
     * Flips all the bits
     */
    void flip() noexcept
	{
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(m_data + i) = ~*(m_data + i);
	}

    /**
     * Flips all the bits in the specified range
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& end) noexcept
    {
		// flip chunks that are in range by bulk, rest flip normally
		for (uint64_t i = 0; i < end / m_chunk_size; ++i)
			*(m_data + i) = ~*(m_data + i);
        for (uint16_t i = 0; i < end % m_chunk_size; ++i)
			*(m_data + end / m_chunk_size) ^= T{ 1 } << i;
    }

    /**
     * Flip all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint64_t to_add = 1;
        if (begin % m_chunk_size)
        {
            for (uint16_t i = begin % m_chunk_size; i < m_chunk_size; ++i)
                *(m_data + begin / m_chunk_size) ^= T{ 1 } << i;
        }
		else
			to_add = 0;

		for (uint64_t i = begin / m_chunk_size + to_add; i < end / m_chunk_size; ++i)
			*(m_data + i) = ~*(m_data + i);

        if (end % m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
				*(m_data + end / m_chunk_size) ^= T{ 1 } << i;
        }
    }

    /**
     * Flips all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to flip (bit step)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            *(m_data + i / m_chunk_size) ^= T{ 1 } << i % m_chunk_size;
        }
    }

    /**
     * Flips the chunk at the specified index
     * @param index Index of the chunk to flip (chunk index)
     */
    void flip_chunk(const uint64_t& index) noexcept
    {
	    *(m_data + index) = ~*(m_data + index);
    }

    /**
     * Flips all the chunks (same as flip())
     */
    void flip_chunk() noexcept
    {
        flip();
    }

    /**
     * Flips all the chunks in the specified range
     * @param end End of the range to fill (chunk index)
     */
    void flip_chunk_in_range(const uint64_t& end)
    {
        for (uint64_t i = 0; i < end; ++i)
            *(m_data + i) = ~*(m_data + i);
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end)
    {
        for (uint64_t i = begin; i < end; ++i)
            *(m_data + i) = ~*(m_data + i);
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
            *(m_data + i) = ~*(m_data + i);
    }

    /**
     * Retrieves the value of a bit at a specified index
     * @param index The index of the bit to read (bit index)
     * @return The value of the bit at the specified index
     */
    [[nodiscard]] bool get(const uint64_t& index) const noexcept
    {
        return *(m_data + index / m_chunk_size) & T { 1 } << index % m_chunk_size;
    }

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] const T& get_chunk(const uint64_t& index) const noexcept
    {
	    return *(m_data + index);
	}

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] T& get_chunk(const uint64_t& index) noexcept
    {
        return *(m_data + index);
    }


    /**
     * Checks if all bits are set 
     * @return true if all bits are set, false otherwise
     */
    [[nodiscard]] bool all() const noexcept
    {
        // check all except the last one if the size is not divisible by m_chunk_size
        for (T* i = m_data; i < m_data + m_storage_size - (m_size % m_chunk_size ? 1 : 0); ++i)
        {
            if (*i != (std::numeric_limits<T>::max)())
                return false;
        }
        if (m_size % m_chunk_size)
        {
            for (uint16_t i = 0; i < m_size % m_chunk_size; ++i)
            {
                if (!(*(m_data + m_size / m_chunk_size) & T{ 1 } << i))
                    return false;
            }
        }
        return true;
    }

    /**
     * Checks if any bit is set
     * @return true if any bit is set, false otherwise
     */
    [[nodiscard]] bool any() const noexcept
    {
        for (T* i = m_data; i < m_data + m_storage_size - (m_size % m_chunk_size ? 1 : 0); ++i)
        {
            if (*i)
                return true;
        }
        if (m_size % m_chunk_size)
		{
			for (uint16_t i = 0; i < m_size % m_chunk_size; ++i)
			{
				if (*(m_data + m_size / m_chunk_size) & T{ 1 } << i)
					return true;
			}
		}
        return false;
    }

    /**
     * Checks if none of the bits are set
     * @return true if none of the bits are set, false otherwise
     */
    [[nodiscard]] bool none() const noexcept
    {
        for (T* i = m_data; i < m_data + m_storage_size; ++i)
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
    [[nodiscard]] bool all_clear() const noexcept
    {
        return none();
    }

    /**
     * @return The number of set bits
     */
    [[nodiscard]] uint64_t count() const noexcept
    {
        uint64_t count = 0;
        for (T* i = m_data; i < m_data + m_storage_size; ++i)
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
     * Checks if the Bitset is empty
     * @return true if the Bitset is empty, false otherwise
     */
    [[nodiscard]] bool empty() const noexcept
	{
        return !m_size;
	}

    /**
     * Pushes back a bit to the Bitset
     * @param value Value of the bit to append (bit value)
     */
    void push_back(const bool& value)
	{
		if (m_size % m_chunk_size)
		{
			if (value)
				*(m_data + m_size / m_chunk_size) |= T{ 1 } << m_size % m_chunk_size;
			else
				*(m_data + m_size / m_chunk_size) &= ~(T{ 1 } << m_size % m_chunk_size);
		}
		else
		{
			T* new_data = new T[m_storage_size + 1];
            if (m_data)
            {
                std::copy(m_data, m_data + m_storage_size, new_data);
                delete[] m_data;
            }
            m_data = new_data;
			*(m_data + m_storage_size++) = value;
		}
        ++m_size;
	}

    /**
     * Removes the last bit from the Bitset
     */
    void pop_back() noexcept
    {
        if (m_data)
        {
            if (!(m_size % m_chunk_size))
            {
                T* new_data = new T[m_storage_size - 1];
                std::copy(m_data, m_data + m_storage_size, new_data);
                delete[] m_data;
                m_data = new_data;
            }
            --m_size;
        }
        // else throw exception in safe version
    }

    /**
     * Pushes back a chunk to the Bitset, adjusting the size to the nearest multiple of sizeof(T) upwards. [e.g. 65 bits -> (+8 {chunk} +7 {expanded area} = +15) -> 80 bits]
     * The bits in the expanded area may be initialized by previous calls, but their values are not explicitly defined by this function.
     * @param chunk The chunk to push back (chunk value)
     */
    void push_back_chunk(const T& chunk) noexcept
    {
        T* new_data = new T[m_storage_size + (m_size % m_chunk_size ? 2 : 1)];
        if (m_data)
        {
            std::copy(m_data, m_data + m_storage_size, new_data);
            delete[] m_data;
        }
        m_data = new_data;
        *(m_data + m_storage_size++) = chunk;
        m_size += m_chunk_size + (m_size % m_chunk_size ? m_chunk_size - m_size % m_chunk_size : 0);
    }

    /**
     * Removes the last chunk from the bitset, adjusting the size to the nearest lower multiple of sizeof(T). [e.g. 65 bits -> 64 bits -> 56 bits]
     */
    void pop_back_chunk() noexcept
    {
        if (m_data)
        {
            if (m_size % m_chunk_size)
                m_size = (m_storage_size - 1) * m_chunk_size;
            
            T* new_data = new T[m_storage_size - 1];
            std::copy(m_data, m_data + m_storage_size - 1, new_data);
            delete[] m_data;
            m_data = new_data;
            --m_storage_size;
            m_size -= m_chunk_size;
        }
        // else throw error in safe version
    }

	/**
	 * Resizes the Bitset to the specified size
	 * @param new_size The new size of the bitset (bit size)
	 */
	void resize(const uint64_t& new_size) noexcept
	{
		if (new_size == m_size)
			return;
		if (new_size < m_size)
		{
			m_size = new_size;
            m_storage_size = calculate_storage_size(m_size);
			return;
		}
		const uint64_t new_storage_size = calculate_storage_size(new_size);
		T* new_data = new T[new_storage_size];
		if (m_data)
		{
			std::copy(m_data, m_data + m_storage_size, new_data);
			delete[] m_data;
		}
        m_data = new_data;
        m_storage_size = new_storage_size;
		m_size = new_size;
	}

    /**
     * Returns the size of the Bitset in bits
     */
    [[nodiscard]] uint64_t size() const noexcept
	{
		return m_size;
	}

    /**
     * Returns the number of chunks the bitset would utilize for given size
     * @param size Size of the target bitset in bits
     * @return The number of chunks the bitset would utilize for the given size
     */
    [[nodiscard]] inline static constexpr uint64_t calculate_storage_size(const uint64_t& size) noexcept
    {
        return size / (sizeof(T) * 8) + (size % (sizeof(T) * 8) ? 1 : 0);
    }

	/**
	 * Creates a chunk of type T based on the given boolean value.
	 *
	 * @param value A boolean value indicating whether to create the chunk with the maximum value or zero.
	 * @return The created chunk of type T. If value is true, returns the maximum value representable by type T,
	 *         otherwise returns zero.
	 */
    [[nodiscard]] inline static constexpr T create_filled_chunk(const bool& value)
    {
        return value ? (std::numeric_limits<T>::max)() : 0u;
    }

	/**
	 * Underlying array of chunks containing the bits
	 */
	alignas(std::hardware_destructive_interference_size) T* m_data;

	/**
	 * Size of the bitset in bits
	 */
	uint64_t m_size;

	/**
	 * Size of the bitset in chunks
	 */
	uint64_t m_storage_size;

	/**
	 * Bit-length of the underlying type
	 */
	static constexpr uint16_t m_chunk_size = sizeof(T) * 8;
};

template <UnsignedInteger T, uint64_t Size>
class CBitSet
{
public:
    /**
     * Empty constructor
     */
	constexpr CBitSet() noexcept : m_data() {}

    /**
     * Initializer list constructor
     * @param list Initializer list to fill the CBitSet with, must contain *chunks* not bits
     */
    constexpr CBitSet(const std::initializer_list<T> list) noexcept
    {
        std::copy(list.begin(), list.end(), m_data);
    }

    /**
	 * Chunk value constructor
	 * @param chunk Chunk to fill the bitset with (chunk value)
	 */
    constexpr CBitSet(const T& chunk) noexcept
	{
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
                m_data[i] = chunk;
        }
        else
			std::fill(m_data, m_data + m_storage_size, chunk);
	}

    /**
     * Copy constructor
     * @param other Other CDynamicBitSet instance to copy from
     */
    constexpr CBitSet(const CBitSet& other) noexcept
    {
        std::copy(other.m_data, other.m_data + other.m_size / m_chunk_size + (other.m_size % m_chunk_size ? 1 : 0), m_data);
    }

    /**
     * Destructor
     */
    constexpr ~CBitSet() noexcept = default;

    /**
     * Returns the value of the bit at the specified index
     * @param index Index of the bit to retrieve (bit index)
     * @return Value of the bit at the specified index (bit value)
     */
    [[nodiscard]] constexpr bool operator[](const uint64_t& index) const noexcept
    {
        return get(index);
    }

    /**
     * Copy assignment operator
     * @param other Other CDynamicBitSet instance to copy from
     * @return Reference to the current instance (self)
     */
    constexpr CBitSet& operator=(const CBitSet& other) noexcept
    {
        if (m_data != other.m_data)
            std::copy(other.m_data, other.m_data + m_storage_size, m_data);
        
        return *this;
    }

    /**
     * Sets the bit at the specified index to the specified value
     * @param value Value to set the bit to (bit value)
     * @param index Index of the bit to set (bit index)
     */
    constexpr void set(const bool& value, const uint64_t& index) noexcept
    {
        if (value)
            m_data[index / m_chunk_size] |= T{ 1 } << index % m_chunk_size;
        else
            m_data[index / m_chunk_size] &= ~(T{ 1 } << index % m_chunk_size);
    }

    /**
     * Sets the bit at the specified index to 1 (true)
     * @param index Index of the bit to set (bit index)
     */
    constexpr void set(const uint64_t& index) noexcept
    {
        m_data[index / m_chunk_size] |= T{1} << index % m_chunk_size;
    }

    /**
     * Sets the bit at the specified index to 0 (false)
     * @param index Index of the bit to clear (bit index)
     */
    constexpr void clear(const uint64_t& index) noexcept
    {
        m_data[index / m_chunk_size] &= ~(T{ 1 } << index % m_chunk_size);
    }

    /**
     * Fills all the bits with the specified value
     * @param value Value to fill the bits with (bit value)
     */
    constexpr void fill(const bool& value) noexcept
    {
        if (std::is_constant_evaluated())
    	{
            for (T& i : m_data)
                i = value ? (std::numeric_limits<T>::max)() : 0;
        }
        else
        {
            ::memset(m_data, value ? 0xFF : 0, m_storage_size * sizeof(T));
        }
    }

    /**
     * Clears all the bits (sets all bits to 0)
     */
    constexpr void clear() noexcept
    {
        if (std::is_constant_evaluated())
        {
			for (T& i : m_data)
                i = 0;
        }
        else
            ::memset(&m_data, 0, sizeof(m_data));
    }

    /**
     * Fills all the bits with 1 (true)
     */
    constexpr void set() noexcept
    {
        if (std::is_constant_evaluated())
        {
            for (T& i : m_data)
                i = (std::numeric_limits<T>::max)();
        }
        else
	        ::memset(m_data, 0xFF, m_storage_size * sizeof(T));
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param end End of the range to fill (bit index)
     */
    constexpr void fill_in_range(const bool& value, const uint64_t& end) noexcept
    {
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = 0; i < end / m_chunk_size; ++i)
                m_data[i] = value ? (std::numeric_limits<T>::max)() : 0;
        }
        else
			::memset(m_data, value ? 255u : 0, end / m_chunk_size * sizeof(T));
        if (end % m_chunk_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                    m_data[end / m_chunk_size] |= T{ 1 } << i;
            }
            else if (!value)
            {
                for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                    m_data[end / m_chunk_size] &= ~(T{ 1 } << i);
            }
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param end End of the range to fill (bit index)
     */
    constexpr void clear_in_range(const uint64_t& end) noexcept
    {
        ::memset(m_data, 0, end / m_chunk_size * sizeof(T));
        if (end % m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                m_data[end / m_chunk_size] &= ~(T{ 1 } << i);
        }
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param end End of the range to fill (bit index)
     */
    constexpr void set_in_range(const uint64_t& end) noexcept
    {
        ::memset(m_data, (std::numeric_limits<T>::max)(), end / m_chunk_size * sizeof(T));
        if (end % m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                m_data[end / m_chunk_size] |= T{ 1 } << i;
        }
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end) noexcept
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % m_chunk_size)
        {
            const uint16_t end_bit = (begin / m_chunk_size == end / m_chunk_size) ? end % m_chunk_size : m_chunk_size;
            if (value)
            {
                for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                    m_data[begin / m_chunk_size] |= T{ 1 } << i;
            }
            else
            {
                for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                    m_data[begin / m_chunk_size] &= ~(T{ 1 } << i);
            }
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % m_chunk_size && begin / m_chunk_size != end / m_chunk_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                    m_data[end / m_chunk_size] |= T{ 1 } << i;
            }
            else
            {
                for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                    m_data[end / m_chunk_size] &= ~(T{ 1 } << i);
            }
        }
        else
            to_sub = 0;
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = begin / m_chunk_size + to_add; i < (end - begin) / m_chunk_size * sizeof(T) - to_sub; ++i)
                m_data[i] = value ? max_value : 0;
        }
        else
			::memset(m_data + begin / m_chunk_size + to_add, value ? max_value : 0, (end - begin) / m_chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void clear_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % m_chunk_size)
        {
            const uint16_t end_bit = begin / m_chunk_size == end / m_chunk_size ? end % m_chunk_size : m_chunk_size;
            for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                m_data[end / m_chunk_size] &= ~(T{ 1 } << i);
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % m_chunk_size && begin / m_chunk_size != end / m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                m_data[end / m_chunk_size] &= ~(T{ 1 } << i);
        }
        else
            to_sub = 0;
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = begin / m_chunk_size + to_add; i < (end - begin) / m_chunk_size * sizeof(T) - to_sub; ++i)
                m_data[i] = 0;
        }
        else
			::memset(m_data + begin / m_chunk_size + to_add, 0, (end - begin) / m_chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void set_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        constexpr T max_value = (std::numeric_limits<T>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_chunk and fill the first byte with it
        if (begin % m_chunk_size)
        {
            const uint16_t end_bit = (begin / m_chunk_size == end / m_chunk_size) ? end % m_chunk_size : m_chunk_size;
            for (uint16_t i = begin % m_chunk_size; i < end_bit; ++i)
                m_data[begin / m_chunk_size] |= T{ 1 } << i;
        }
        else
            to_add = 0;

        // set the end chunk if the end is not aligned with the chunk size
        if (end % m_chunk_size && begin / m_chunk_size != end / m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                m_data[end / m_chunk_size] |= T{ 1 } << i;
        }
        else
            to_sub = 0;
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = begin / m_chunk_size + to_add; i < (end - begin) / m_chunk_size * sizeof(T) - to_sub; ++i)
                m_data[i] = max_value;
        }
        else 
            ::memset(m_data + begin / m_chunk_size + to_add, max_value, (end - begin) / m_chunk_size * sizeof(T) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    constexpr void fill_in_range(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            if (value)
                m_data[i / m_chunk_size] |= T{ 1 } << i % m_chunk_size;
            else
                m_data[i / m_chunk_size] &= ~(T{ 1 } << i % m_chunk_size);
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    constexpr void clear_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i / m_chunk_size] &= ~(T{ 1 } << i % m_chunk_size);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    constexpr void set_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i / m_chunk_size] |= T{ 1 } << i % m_chunk_size;
    }

    /**
     * !!! W.I.P. - Does not function correctly at the moment !!!\n
     * Fill the bits in the specified range with the specified value using an optimized algorithm.\n
     * This algorithm is particularly efficient when the step size is relatively low.\n
     * Note: This function has a rather complex implementation. It is not recommended to use it when simple filling without a step is possible.\n
     * Performance of this function varies significantly depending on the step. It performs best when step is a multiple of m_chunk_size, and is within reasonable range from it.\n
     * However, worst when step is not aligned with m_chunk_size and end is not aligned with m_chunk_size. In such cases, extra processing is required to handle the boundary chunks.\n
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    constexpr void fill_in_range_optimized(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        // Initialize variables
        uint64_t chunks_size, current_chunk = begin / m_chunk_size + 1 + step / m_chunk_size, current_offset = 0;
        uint16_t offset;
        const uint64_t end_chunk = end / m_chunk_size + (end % m_chunk_size ? 1 : 0);

        // Determine the size of chunks based on step and chunk size
        if ((step % 2 || step <= m_chunk_size) && m_chunk_size % step) {
            chunks_size = (std::min)(m_storage_size, step);
        }
        else if (!(m_chunk_size % step))
            chunks_size = 1;
        else if (!(step % m_chunk_size))
            chunks_size = step / m_chunk_size;
        else
        {
            // GCD of step and m_chunk_size
            if (step % m_chunk_size)
            {
                uint64_t a = step, b = m_chunk_size, t = m_chunk_size;
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
        if (begin < m_chunk_size)
        {
            offset = (m_chunk_size - begin) % step;
            if (offset)
                offset = step - offset;
        }
        else
        {
            offset = (begin - m_chunk_size) % step;
        }

        if (offset)
            offset = (m_chunk_size - offset + step / m_chunk_size * m_chunk_size) % step;

        std::cout << "offset: " << offset << '\n';

        // Create and apply the beginning chunk
        {
            const uint16_t end_bit = (begin / m_chunk_size == end / m_chunk_size) ? end % m_chunk_size : m_chunk_size;
            if (value)
            {
                for (uint16_t i = begin % m_chunk_size; i < end_bit; i += step)
                    *(m_data + begin / m_chunk_size) |= T{ 1 } << i;
            }
            else
            {
                for (uint16_t i = begin % m_chunk_size; i < end_bit; i += step)
                    *(m_data + begin / m_chunk_size) &= ~(T{ 1 } << i);
            }
        }


        // Fill with appropriate chunk
        std::cout << chunks_size << " chunks\n";
        std::cout << (std::min)(chunks_size + begin / m_chunk_size, m_storage_size) << " chunks\n";
        for (uint64_t i = 0; i < (std::min)(chunks_size, m_storage_size); ++i)
        {
            // Generate chunk for the current iteration
            T chunk = 0;

            if (value)
            {
                for (uint16_t j = !i ? offset : 0; j < m_chunk_size; ++j)
                {
                    std::cout << current_chunk * m_chunk_size + j - offset << '\n';
                    if (!((current_chunk * m_chunk_size + j - offset) % step))
                        chunk |= T{ 1 } << j;
                }
            }
            else
            {
                chunk = (std::numeric_limits<T>::max)();
                for (uint16_t j = (!i ? offset : 0); j < m_chunk_size; ++j)
                {
                    if (!((current_chunk * m_chunk_size + j - offset) % step))
                        chunk &= ~(T{ 1 } << j);
                }
            }

            // print the chunk
            for (uint16_t j = 0; j < m_chunk_size; ++j)
            {
                std::cout << ((chunk & T{ 1 } << j) >> j);
            }

            std::cout << '\n';

            // Apply the chunk
            for (uint64_t j = current_chunk; j < m_storage_size; ++j)
            {
                if (j == end_chunk - 1 && end % m_chunk_size)
                {
                    // Remove bits that overflow the range
                    if (value)
                    {
                        for (uint16_t k = end % m_chunk_size; k < m_chunk_size; ++k)
                            chunk &= ~(T{ 1 } << k);
                        *(m_data + j) |= chunk;
                    }
                    else
                    {
                        for (uint16_t k = end % m_chunk_size; k < m_chunk_size; ++k)
                            chunk |= T{ 1 } << k;
                        *(m_data + j) &= chunk;
                    }
                    break;
                }
                if (value)
                    *(m_data + j) |= chunk;
                else
                    *(m_data + j) &= chunk;
            }
            ++current_chunk;
        }
    }

    /**
     * !!! W.I.P. - May not choose the best option, not even talking about the fact that set_in_range_optimized function doesn't even work correctly !!!\n
     * Fill the bits in the specified range with the specified value.\n
     * Chooses the fastest implementation based on the step.\n
     * This function becomes more accurate in choosing the fastest implementation as the size of the Bitset increases.\n
     * @param value Value to fill the bits with
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill
    */
    constexpr void set_in_range_fastest(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        if (step == 1)
        {
            set_in_range(value, begin, end);
            return;
        }
        if (step <= m_chunk_size * 2.5) // approximately up until this point it is faster, though no scientific anything went into this, just a guess lol
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
    constexpr void set_chunk(const T& chunk, const uint64_t& index) noexcept
    {
        m_data[index] = chunk;
    }

    /**
     * Fills all the chunks with the specified chunk
     * @param chunk Chunk to fill the chunks with (chunk value)
     */
    constexpr void fill_chunk(const T& chunk) noexcept
    {
        for (uint64_t i = 0; i < m_storage_size; ++i)
            m_data[i] = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param end End of the range to fill (chunk index)
     */
    constexpr void fill_chunk_in_range(const T& chunk, const uint64_t& end) noexcept
    {
        for (uint64_t i = 0; i < end; ++i)
            m_data[i] = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    constexpr void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end) noexcept
    {
        for (uint64_t i = begin; i < end; ++i)
            m_data[i] = chunk;
    }

    /**
     * Fills all the bits in the specified range with the specified chunk
     * @param chunk Chunk to fill the bits with (chunk value)
     * @param begin begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to fill (chunk step)
     */
    constexpr void fill_chunk_in_range(const T& chunk, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i] = chunk;
    }

    /**
     * Flips the bit at the specified index
     * @param index Index of the bit to flip (bit index)
     */
    constexpr void flip(const uint64_t& index) noexcept
    {
        m_data[index / m_chunk_size] ^= T{ 1 } << index % m_chunk_size;
    }

    /**
     * Flips all the bits
     */
    constexpr void flip() noexcept
    {
        for (uint64_t i = 0; i < m_storage_size; ++i)
            m_data[i] = ~m_data[i];
    }

    /**
     * Flips all the bits in the specified range
     * @param end End of the range to fill (bit index)
     */
    constexpr void flip_in_range(const uint64_t& end) noexcept
    {
        // flip chunks that are in range by bulk, rest flip normally
        for (uint64_t i = 0; i < end / m_chunk_size; ++i)
            m_data[i] = ~m_data[i];
        for (uint16_t i = 0; i < end % m_chunk_size; ++i)
            m_data[end / m_chunk_size] ^= T{ 1 } << i;
    }

    /**
     * Flip all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void flip_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint64_t to_add = 1;
        if (begin % m_chunk_size)
        {
            for (uint16_t i = begin % m_chunk_size; i < m_chunk_size; ++i)
                m_data[begin / m_chunk_size] ^= T{ 1 } << i;
        }
        else
            to_add = 0;

        for (uint64_t i = begin / m_chunk_size + to_add; i < end / m_chunk_size; ++i)
            m_data[i] = ~m_data[i];

        if (end % m_chunk_size)
        {
            for (uint16_t i = 0; i < end % m_chunk_size; ++i)
                m_data[end / m_chunk_size] ^= T{ 1 } << i;
        }
    }

    /**
     * Flips all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to flip (bit step)
     */
    constexpr void flip_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i] ^= T{ 1 } << i % m_chunk_size;
    }

    /**
     * Flips the chunk at the specified index
     * @param index Index of the chunk to flip (chunk index)
     */
    constexpr void flip_chunk(const uint64_t& index) noexcept
    {
        m_data[index] = ~m_data[index];
    }

    /**
     * Flips all the chunks (same as flip())
     */
    constexpr void flip_chunk() noexcept
    {
        flip();
    }

    /**
     * Flips all the chunks in the specified range
     * @param end End of the range to fill (chunk index)
     */
    constexpr void flip_chunk_in_range(const uint64_t& end) noexcept
    {
        for (uint64_t i = 0; i < end; ++i)
            m_data[i] = ~m_data[i];
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     */
    constexpr void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        for (uint64_t i = begin; i < end; ++i)
            m_data[i] = ~m_data[i];
    }

    /**
     * Flips all the chunks in the specified range
     * @param begin Begin of the range to fill (chunk index)
     * @param end End of the range to fill (chunk index)
     * @param step Step size between the bits to flip (chunk step)
     */
    constexpr void flip_chunk_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i] = ~m_data[i];
    }

    /**
     * Retrieves the value of a bit at a specified index
     * @param index The index of the bit to read (bit index)
     * @return The value of the bit at the specified index
     */
    [[nodiscard]] constexpr bool get(const uint64_t& index) const noexcept
    {
        return m_data[index / m_chunk_size] & T{ 1 } << index % m_chunk_size;
    }

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] constexpr const T& get_chunk(const uint64_t& index) const noexcept
    {
        return m_data[index];
    }

    /**
     * Retrieves the chunk at the specified index
     * @param index Index of the chunk to retrieve (chunk index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] constexpr T& get_chunk(const uint64_t& index) noexcept
    {
        return m_data[index];
    }


    /**
     * Checks if all bits are set
     * @return true if all bits are set, false otherwise
     */
    [[nodiscard]] constexpr bool all() const noexcept
    {
        // check all except the last one if the size is not divisible by m_chunk_size
        for (uint64_t i = 0; i < m_storage_size - (m_size % m_chunk_size ? 1 : 0); ++i)
        {
	        if (m_data[i] != (std::numeric_limits<T>::max)())
				return false;
		}
        if (m_size % m_chunk_size)
        {
            for (uint16_t i = 0; i < m_size % m_chunk_size; ++i)
            {
                if (!(m_data[m_size / m_chunk_size] & T{ 1 } << i))
                    return false;
            }
        }
        return true;
    }

    /**
     * Checks if any bit is set
     * @return true if any bit is set, false otherwise
     */
    [[nodiscard]] constexpr bool any() const noexcept
    {
		for (uint64_t i = 0; i < m_storage_size - (m_size % m_chunk_size ? 1 : 0); ++i)
		{
			if (m_data[i])
				return true;
		}
		if (m_size % m_chunk_size)
		{
			for (uint16_t i = 0; i < size % m_chunk_size; ++i)
			{
				if (m_data[m_size / m_chunk_size] & T{ 1 } << i)
					return true;
			}
		}
		return false;
	}

	/**
	 *	 * Checks if none of the bits are set
	 *	 * @return true if none of the bits are set, false otherwise
	 *	 */
	[[nodiscard]] constexpr bool none() const noexcept
	{
		for (uint64_t i = 0; i < m_storage_size - (m_size % m_chunk_size ? 1 : 0); ++i)
		{
			if (m_data[i])
				return false;
		}
        if (m_size % m_chunk_size)
        {
            for (uint16_t i = 0; i < m_size % m_chunk_size; ++i)
            {
                if (m_data[m_size / m_chunk_size] & T{ 1 } << i)
                    return true;
            }
        }
        return false;
    }


    /**
     * Checks if all bits are cleared (none are set)
     * @return true if all bits are cleared, false otherwise
     */
    [[nodiscard]] constexpr bool all_clear() const noexcept
    {
        return none();
    }

    /**
     * @return The number of set bits
     */
    [[nodiscard]] constexpr uint64_t count() const noexcept
    {
        uint64_t count = 0;
        for (T* i = m_data; i < m_data + m_storage_size; ++i)
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
     * Checks if the Bitset is empty
     * @return true if the Bitset is empty, false otherwise
     */
    [[nodiscard]] static constexpr bool empty() noexcept
    {
        return !Size;
    }

    /**
     * Returns the m_size of the Bitset in bits
     * @return Size of the Bitset in bits
     */
	[[nodiscard]] static constexpr uint64_t size() noexcept
    {
	    return Size;
	}

    /**
     * Returns the number of chunks the Bitset would utilize for given size
     * @param size Size of the target Bitset in bits
     * @return The number of chunks the Bitset would utilize for the given size
     */
    [[nodiscard]] inline static constexpr uint64_t calculate_storage_size(const uint64_t& size) noexcept
    {
        return m_size / (sizeof(T) * 8) + (size % (sizeof(T) * 8) ? 1 : 0);
    }

    /**
     * Creates a chunk of type T based on the given boolean value.
     *
     * @param value A boolean value indicating whether to create the chunk with the maximum value or zero.
     * @return The created chunk of type T. If value is true, returns the maximum value representable by type T,
     *         otherwise returns zero.
     */

    [[nodiscard]] inline static constexpr T create_filled_chunk(const bool& value)
    {
        return value ? (std::numeric_limits<T>::max)() : 0u;
    }

    /**
     * Size of the Bitset in bits
     */
    static constexpr uint64_t m_size = Size;

    /**
     * Size of the Bitset in chunks
     */
    static constexpr uint64_t m_storage_size = calculate_storage_size(m_size);

    /**
     * Bit-length of the underlying type
     */
    static constexpr uint16_t m_chunk_size = sizeof(T) * 8;

    /**
	 * Underlying array of chunks containing the bits
	 */
    alignas(std::hardware_destructive_interference_size) T m_data[m_storage_size] = { 0 };
};