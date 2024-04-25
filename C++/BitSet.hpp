#pragma once
#include <algorithm>
#include <cstdint>
#include <limits>
#include <iostream>
#include <concepts>
#include <new>
#include <cstring>

// Note: (std::numeric_limits<BlockType>::max)() is used instead of std::numeric_limits<BlockType>::max() because Windows.h defines a macro max which conflicts with std::numeric_limits<BlockType>::max()

template <typename T>
concept UnsignedInteger = std::is_unsigned_v<T> && std::is_integral_v<T>;

template <UnsignedInteger BlockType>
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
     * @param list Initializer list to fill the Bitset with, containing block values
	 */
    CDynamicBitSet(const uint64_t& size, const std::initializer_list<BlockType> list) noexcept : m_data(new BlockType[calculate_storage_size(size)]), m_size(size), m_storage_size(calculate_storage_size(size))
    {
        std::copy(list.begin(), list.end(), m_data);
    }

    /**
     * Initializer list constructor
     * @param list Initializer list to fill the Bitset with, containing block values
     */
    CDynamicBitSet(const std::initializer_list<BlockType> list) noexcept : m_data(new BlockType[calculate_storage_size(list.size())]), m_size(list.m_size()), m_storage_size(calculate_storage_size(list.size()))
    {
        std::copy(list.begin(), list.end(), m_data);
    }

    /**
     * Size constructor
     * @param size Size of the Bitset to be created (bit count)
     */
    CDynamicBitSet(const uint64_t& size) noexcept : m_data(new BlockType[calculate_storage_size(size)]), m_size(size), m_storage_size(calculate_storage_size(size))
    {
        reset();
    }

    /**
     * Size and block value constructor
     * @param size Size of the bitset to be created (bit size)
     * @param block Chunk to fill the bitset with (block value)
     */
    CDynamicBitSet(const uint64_t& size, const BlockType& block) noexcept : m_data(new BlockType[calculate_storage_size(size)]), m_size(size), m_storage_size(calculate_storage_size(size))
	{
        fill_block(block);
	}

    /**
     * Copy constructor
     * @param other Other CDynamicBitSet instance to copy from
     */
    CDynamicBitSet(const CDynamicBitSet& other) noexcept : m_data(new BlockType[other.m_storage_size]), m_size(other.m_size), m_storage_size(other.m_storage_size)
    {
        std::copy(other.m_data, other.m_data + other.m_storage_size, m_data);
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
        return *(m_data + index / m_block_size) & BlockType{ 1 } << (m_block_size - index % m_block_size);
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
                m_storage_size = other.m_storage_size;
                m_size = other.m_size;
                m_data = new BlockType[other.m_storage_size];
			}
			std::copy(other.m_data, other.m_data + other.m_storage_size, m_data);
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

    // comparison operators

    /**
     * Equality operator
     * @param other Other CDynamicBitSet instance to compare with
     * @return True if the two instances are equal, false otherwise
     */
	[[nodiscard]] bool operator==(const CDynamicBitSet& other) const noexcept
	{
		if (m_size != other.m_size)
			return false;
		for (uint64_t i = 0; i < m_storage_size; ++i)
		{
			if (*(m_data + i) != *(other.m_data + i))
				return false;
		}
		return true;
	}

    /**
     * Inequality operator
     * @param other Other CDynamicBitSet instance to compare with
     * @return True if the two instances are not equal, false otherwise
     */
    [[nodiscard]] bool operator!=(const CDynamicBitSet& other) const noexcept
	{
		if (m_size != other.m_size)
			return true;
        for (uint64_t i = 0; i < m_storage_size; ++i)
        {
	        if (*(m_data + i) != *(other.m_data + i))
				return true;
		}
        return false;
	}

    // Bitwise operators

    /**
     * Bitwise AND operator
     * @param other Other CDynamicBitSet instance to perform the operation with
     * @return New CDynamicBitSet instance containing the result of the operation
     */
	[[nodiscard]] CDynamicBitSet&& operator&(const CDynamicBitSet& other) const noexcept
	{
		CDynamicBitSet result(m_size < other.m_size ? m_size : other.m_size);
		for (uint64_t i = 0; i < result.m_storage_size; ++i)
			*(result.m_data + i) = *(m_data + i) & *(other.m_data + i);
		return result;
	}

    /**
     * Apply bitwise AND operation with another CDynamicBitSet instance
     * @param other Other CDynamicBitSet instance to perform the operation with
     */
    CDynamicBitSet& operator&=(const CDynamicBitSet& other) noexcept
	{
		for (uint64_t i = 0; i < m_storage_size; ++i)
			*(m_data + i) &= *(other.m_data + i);
		return *this;
	}

    /**
     * Bitwise OR operator
     * @param other Other CDynamicBitSet instance to perform the operation with
     * @return New CDynamicBitSet instance containing the result of the operation
     */
	[[nodiscard]] CDynamicBitSet&& operator|(const CDynamicBitSet& other) const noexcept
	{
		CDynamicBitSet result(m_size > other.m_size ? m_size : other.m_size);
		for (uint64_t i = 0; i < result.m_storage_size; ++i)
			*(result.m_data + i) = *(m_data + i) | *(other.m_data + i);
		return result;
	}

    /**
     * Apply bitwise OR operation with another CDynamicBitSet instance
     * @param other Other CDynamicBitSet instance to perform the operation with
     */
    CDynamicBitSet& operator|=(const CDynamicBitSet& other) noexcept
	{
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(m_data + i) |= *(other.m_data + i);
        return *this;
	}

    /**
	 * Bitwise XOR operator
	 * @param other Other CDynamicBitSet instance to perform the operation with
	 * @return New CDynamicBitSet instance containing the result of the operation
	 */
    [[nodiscard]] CDynamicBitSet&& operator^(const CDynamicBitSet& other) const noexcept
    {
        CDynamicBitSet result(m_size > other.m_size ? m_size : other.m_size);
        for (uint64_t i = 0; i < result.m_storage_size; ++i)
            *(result.m_data + i) = *(m_data + i) ^ *(other.m_data + i);
        return result;
    }

    /**
	 * Apply bitwise XOR operation with another CDynamicBitSet instance
	 * @param other Other CDynamicBitSet instance to perform the operation with
	 */
    CDynamicBitSet& operator^=(const CDynamicBitSet& other) noexcept
    {
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(m_data + i) ^= *(other.m_data + i);
        return *this;
    }

    /**
	 * Bitwise NOT operator
	 * @return New CDynamicBitSet instance containing the result of the operation
	 */
    [[nodiscard]] CDynamicBitSet&& operator~() const noexcept
    {
        CDynamicBitSet result(m_size);
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(result.m_data + i) = ~*(m_data + i);
        return result;
    }

    /**
     * Bitwise right shift operator
     * @param shift Amount of bits to shift to the right
     * @return New CDynamicBitSet instance containing the result of the operation
     */
    [[nodiscard]] CDynamicBitSet&& operator>>(const uint64_t& shift) const noexcept
	{
        CDynamicBitSet result(m_size);
        if (shift <= m_block_size)
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
                *(result.m_data + i) = *(m_data + i) >> shift;
        }
        return result;
	}

    /**
	 * Apply bitwise right shift operation
	 * @param shift Amount of bits to shift to the right
	 */
    [[nodiscard]] CDynamicBitSet& operator>>=(const uint64_t& shift) noexcept
    {
        if (shift > m_block_size)
            reset();
        else
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
                *(m_data + i) >>= shift;
        }
        return *this;
    }


    /**
     * Bitwise right shift operator
     * @param shift Amount of bits to shift to the right
     * @return New CDynamicBitSet instance containing the result of the operation
     */
    [[nodiscard]] CDynamicBitSet&& operator<<(const uint64_t& shift) const noexcept
    {
        CDynamicBitSet result(m_size);
        if (shift <= m_block_size)
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
                *(result.m_data + i) = *(m_data + i) << shift;
        }
        return result;
    }

    /**
     * Apply bitwise right shift operation
     * @param shift Amount of bits to shift to the right
     */
    [[nodiscard]] CDynamicBitSet& operator<<=(const uint64_t& shift) noexcept
    {
        if (shift > m_block_size)
            reset();
        else
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
                *(m_data + i) <<= shift;
        }
        return *this;
    }

    /**
     * Difference operator
     * @param other Other CDynamicBitSet instance to compare with
     * @return New CDynamicBitSet instance containing the result of the operation
     */
	[[nodiscard]] CDynamicBitSet&& operator-(const CDynamicBitSet& other) const noexcept
	{
		CDynamicBitSet result(m_size < other.m_size ? m_size : other.m_size);
        for (uint64_t i = 0; i < result.m_storage_size; ++i)
			*(result.m_data + i) = *(m_data + i) & ~*(other.m_data + i);
	}

    /**
     *  Apply difference operation
     * @param other Other CDynamicBitSet instance to compare with
     */
    CDynamicBitSet& operator-=(const CDynamicBitSet& other) noexcept
	{
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(m_data + i) &= ~*(other.m_data + i);
	}

    // Utility functions

    /**
     * Converts the Bitset to a string
     * @param set_chr Character to represent set bits
     * @param clr_chr Character to represent clear bits
     * @param separator Character to separate the bit blocks
     * @return String representation of the Bitset
     */
    [[nodiscard]] std::string&& to_string(const char& set_chr = '1', const char& clr_chr = '0', const char& separator = 0) const noexcept
	{
        std::string result("", m_size + (separator ? m_storage_size : 0));
		std::string::iterator it = result.begin();
        if (!separator)
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
            {
                for (uint16_t j = 0; j < m_block_size; ++j)
                    *it++ = *(m_data + i) & BlockType { 1 } << j ? set_chr : clr_chr;
            }
        }
        else
        {
	        for (uint64_t i = 0; i < m_storage_size; ++i)
	        {
	        	for (uint16_t j = 0; j < m_block_size; ++j)
					*it++ = *(m_data + i) & BlockType { 1 } << j ? set_chr : clr_chr;
				*it++ = separator;
			}
        }
		return std::move(result);
	}

	/**
	 * Converts the Bitset to an integer value
	 * @tparam T Type of the integral value to convert to
	 * @return Converted integer value
	 */
	template <UnsignedInteger T>
	[[nodiscard]] T to_integer() const noexcept
	{
		T result = 0;
		for (uint64_t i = 0; i < m_storage_size; ++i)
			result |= *(m_data + i);
		return result;
	}
    /**
     * @return Size of the Bitset (bit count)
     */
    [[nodiscard]] uint64_t size() const noexcept { return m_size; }

	/**
	 * @return Number of blocks in the Bitset
	 */
	[[nodiscard]] uint64_t storage_size() const noexcept { return m_storage_size; }

	/**
	 * @return Pointer to the underlying array 
	 */
	[[nodiscard]] BlockType* data() noexcept { return m_data; }

    /**
	 * @return Const pointer to the underlying array
	 */
    [[nodiscard]] const BlockType* data() const noexcept { return m_data; }

    // Bitset operations

    /**
     * Sets the bit at the specified index to the specified value
     * @param value Value to set the bit to (bit value)
     * @param index Index of the bit to set (bit index)
     */
    void set(const uint64_t& index, const bool& value = true) noexcept
    {
        if (value)
            *(m_data + index / m_block_size) |= BlockType{ 1 } << (m_block_size - index % m_block_size);
        else
            *(m_data + index / m_block_size) &= ~(BlockType{ 1 } << (m_block_size - index % m_block_size));
    }

    /**
     * Sets the bit at the specified index to 0 (false)
     * @param index Index of the bit to reset (bit index)
     */
    void reset(const uint64_t& index) noexcept
    {
        *(m_data + index / m_block_size) &= ~(BlockType{ 1 } << (m_block_size - index % m_block_size));
    }

    /**
     * Fills all the bits with the specified value
     * @param value Value to fill the bits with (bit value)
     */
    void fill(const bool& value) noexcept
    {
        ::memset(m_data, value ? (std::numeric_limits<BlockType>::max)() : 0, m_storage_size * sizeof(BlockType));
    }

    /**
     * Clears all the bits (sets all bits to 0)
     */
    void reset() noexcept
    {
        ::memset(m_data, 0, m_storage_size * sizeof(BlockType));
    }

    /**
     * Fills all the bits with 1 (true)
     */
    void set() noexcept
    {
        ::memset(m_data, (std::numeric_limits<BlockType>::max)(), m_storage_size * sizeof(BlockType));
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const uint64_t& end, const bool& value = true) noexcept
    {
        ::memset(m_data, value ? (std::numeric_limits<BlockType>::max)() : 0, end / m_block_size * sizeof(BlockType));
        if (end % m_block_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % m_block_size; ++i)
					*(m_data + end / m_block_size) |= BlockType{ 1 } << i;
            }
            else if (!value)
            {
                for (uint16_t i = 0; i < end % m_block_size; ++i)
                    *(m_data + end / m_block_size) &= ~(BlockType{ 1 } << i);
            }
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param end End of the range to fill (bit index)
     */
    void reset_in_range(const uint64_t& end) noexcept
    {
        ::memset(m_data, 0, end / m_block_size * sizeof(BlockType));
        if (end % m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                *(m_data + end / m_block_size) &= ~(BlockType{ 1 } << i);
        }
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void fill_in_range(const uint64_t& begin, const uint64_t& end, const bool& value = true) noexcept
    {
        constexpr BlockType max_value = (std::numeric_limits<BlockType>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_block and fill the first byte with it
        if (begin % m_block_size)
        {
            const uint16_t end_bit = (begin / m_block_size == end / m_block_size) ? end % m_block_size : m_block_size;
            if (value)
            {
                for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                    *(m_data + begin / m_block_size) |= BlockType{ 1 } << i;
            }
            else
            {
                for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                    *(m_data + begin / m_block_size) &= ~(BlockType{ 1 } << i);
            }
        }
		else
			to_add = 0;

        // set the end block if the end is not aligned with the block size
        if (end % m_block_size && begin / m_block_size != end / m_block_size)
        {
	        if (value)
	        {
	        	for (uint16_t i = 0; i < end % m_block_size; ++i)
					*(m_data + end / m_block_size) |= BlockType{ 1 } << i;
			}
			else
			{
				for (uint16_t i = 0; i < end % m_block_size; ++i)
					*(m_data + end / m_block_size) &= ~(BlockType{ 1 } << i);
			}
		}
        else
            to_sub = 0;

        ::memset(m_data + begin / m_block_size + to_add, value ? max_value : 0, (end - begin) / m_block_size * sizeof(BlockType) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void reset_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint8_t to_add = 1, to_sub = 1;
        // create begin_block and fill the first byte with it
        if (begin % m_block_size)
        {
            const uint16_t end_bit = begin / m_block_size == end / m_block_size ? end % m_block_size : m_block_size;
            for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                *(m_data + begin / m_block_size) &= ~(BlockType{ 1 } << i);
        }
        else
            to_add = 0;

        // set the end block if the end is not aligned with the block size
        if (end % m_block_size && begin / m_block_size != end / m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                *(m_data + end / m_block_size) &= ~(BlockType{ 1 } << i);
        }
        else
            to_sub = 0;

        ::memset(m_data + begin / m_block_size + to_add, 0, (end - begin) / m_block_size * sizeof(BlockType) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with the specified value
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step, const bool& value = true) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
        {
            if (value)
                *(m_data + i / m_block_size) |= BlockType{ 1 } << i % m_block_size;
            else
                *(m_data + i / m_block_size) &= ~(BlockType{ 1 } << i % m_block_size);
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void reset_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            *(m_data + i / m_block_size) &= ~(BlockType{ 1 } << i % m_block_size);
    }

    /**
     * !!! W.I.P. - Does not function correctly at the moment !!!\n
     * Fill the bits in the specified range with the specified value using an optimized algorithm.\n
     * This algorithm is particularly efficient when the step size is relatively low.\n
     * Note: This function has a rather complex implementation. It is not recommended to use it when simple filling without a step is possible.\n
     * Performance of this function varies significantly depending on the step. It performs best when step is a multiple of m_block_size, and is within reasonable range from it.\n
     * However, worst when step is not aligned with m_block_size and end is not aligned with m_block_size. In such cases, extra processing is required to handle the boundary blocks.\n
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    void fill_in_range_optimized(const uint64_t& begin, const uint64_t& end, const uint64_t& step, const bool& value = true) noexcept
    {
        // Initialize variables
        uint64_t blocks_size, current_block = begin / m_block_size + 1 + step / m_block_size, current_offset = 0;
        uint16_t offset;
        const uint64_t end_block = end / m_block_size + (end % m_block_size ? 1 : 0);

        // Determine the size of blocks based on step and block size
        if ((step % 2 || step <= m_block_size) && m_block_size % step) {
            blocks_size = (std::min)(m_storage_size, step);
        }
        else if (!(m_block_size % step))
            blocks_size = 1;
        else if (!(step % m_block_size))
            blocks_size = step / m_block_size;
        else
        {
            // GCD of step and m_block_size
            if (step % m_block_size)
            {
                uint64_t a = step, b = m_block_size, t = m_block_size;
                while (b) {
                    t = b;
                    b = a % b;
                    a = t;
                }
                blocks_size = a;
            }
            else
                blocks_size = 1;
        }

        // Calculate the offset
        if (begin < m_block_size)
        {
            offset = (m_block_size - begin) % step;
            if (offset)
                offset = step - offset;
        }
        else
        {
            offset = (begin - m_block_size) % step;
        }

        if (offset)
			offset = (m_block_size - offset + step / m_block_size * m_block_size) % step;

        std::cout << "offset: " << offset << '\n';

        // Create and apply the beginning block
		{
			const uint16_t end_bit = (begin / m_block_size == end / m_block_size) ? end % m_block_size : m_block_size;
        	if (value)
        	{
        		for (uint16_t i = begin % m_block_size; i < end_bit; i += step)
        			*(m_data + begin / m_block_size) |= BlockType{ 1 } << i;
        	}
        	else
        	{
        		for (uint16_t i = begin % m_block_size; i < end_bit; i += step)
        			*(m_data + begin / m_block_size) &= ~(BlockType{ 1 } << i);
        	}
		}


        // Fill with appropriate block
        std::cout << blocks_size << " blocks\n";
        std::cout << (std::min)(blocks_size + begin / m_block_size, m_storage_size) << " blocks\n";
        for (uint64_t i = 0; i < (std::min)(blocks_size, m_storage_size); ++i)
        {
            // Generate block for the current iteration
            BlockType block = 0;

            if (value)
            {
                for (uint16_t j = !i ? offset : 0; j < m_block_size; ++j)
                {
                    std::cout << current_block * m_block_size + j - offset << '\n';
                    if (!((current_block * m_block_size + j - offset) % step))
                        block |= BlockType{ 1 } << j;
                }
            }
            else
            {
                block = (std::numeric_limits<BlockType>::max)();
                for (uint16_t j = (!i ? offset : 0); j < m_block_size; ++j)
                {
                    if (!((current_block * m_block_size + j - offset) % step))
                        block &= ~(BlockType{ 1 } << j);
                }
            }

            // print the block
            for (uint16_t j = 0; j < m_block_size; ++j)
            {
                std::cout << ((block & BlockType{ 1 } << j) >> j);
            }

            std::cout << '\n';

            // Apply the block
            for (uint64_t j = current_block; j < m_storage_size; ++j)
            {
	            if (j == end_block - 1 && end % m_block_size)
	            {
	            	// Remove bits that overflow the range
					if (value)
					{
						for (uint16_t k = end % m_block_size; k < m_block_size; ++k)
							block &= ~(BlockType{ 1 } << k);
						*(m_data + j) |= block;
					}
					else
					{
						for (uint16_t k = end % m_block_size; k < m_block_size; ++k)
							block |= BlockType{ 1 } << k;
						*(m_data + j) &= block;
					}
					break;
				}
				if (value)
					*(m_data + j) |= block;
				else
					*(m_data + j) &= block;
			}
            ++current_block;
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
    void fill_in_range_fastest(const uint64_t& begin, const uint64_t& end, const uint64_t& step, const bool& value = true) noexcept
    {
        if (step == 1)
        {
            fill_in_range(begin, end, value);
            return;
        }
        if (step <= m_block_size * 2.5) // approximately up until this point it is faster, though no scientific anything went into this, just a guess lol
        {
            fill_in_range_optimized(value, begin, end, step);
            return;
        }
        fill_in_range(value, begin, end, step);
    }

    /**
     * Sets the block at the specified index to the specified value (default is max value, all bits set to 1)
     * @param block Chunk to set (block value)
     * @param index Index of the block to set (block index)
     */
    void set_block(const uint64_t& index, const BlockType& block = (std::numeric_limits<BlockType>::max)()) noexcept
    {
        *(m_data + index) = block;
    }

	/**
	 * Sets the block at the specified index to 0 (all bits set to 0)
	 * @param index Index of the block to reset (block index)
	 */
	void reset_block(const uint64_t& index) noexcept
    {
        *(m_data + index) = 0u;
    }

    /**
     * Fills all the blocks with the specified block
     * @param block Chunk to fill the blocks with (block value)
     */
    void fill_block(const BlockType& block) noexcept
    {
        for (uint64_t i = 0; i < m_storage_size; ++i)
            *(m_data + i) = block;
    }

    /**
     * Fills all the bits in the specified range with the specified block
     * @param block Chunk to fill the bits with (block value)
     * @param end End of the range to fill (block index)
     */
    void fill_block_in_range(const uint64_t& end, const BlockType& block = (std::numeric_limits<BlockType>::max)()) noexcept
    {
        for (uint64_t i = 0; i < end; ++i)
			*(m_data + i) = block;
    }

    /**
     * Fills all the bits in the specified range with the specified block
     * @param block Chunk to fill the bits with
     * @param begin begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     */
    void fill_block_in_range(const uint64_t& begin, const uint64_t& end, const BlockType& block = (std::numeric_limits<BlockType>::max)()) noexcept
    {
        for (uint64_t i = begin; i < end; ++i)
            *(m_data + i) = block;
    }

    /**
     * Fills all the bits in the specified range with the specified block
     * @param block Chunk to fill the bits with (block value)
     * @param begin begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     * @param step Step size between the bits to fill (block step)
     */
    void fill_block_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step, const BlockType& block = (std::numeric_limits<BlockType>::max)()) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            *(m_data + i) = block;
    }

    /**
     * Flips the bit at the specified index
     * @param index Index of the bit to flip (bit index)
     */
    void flip(const uint64_t& index) noexcept
    {
	    *(m_data + index / m_block_size) ^= BlockType{ 1 } << (m_block_size - index % m_block_size);
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
		// flip blocks that are in range by bulk, rest flip normally
		for (uint64_t i = 0; i < end / m_block_size; ++i)
			*(m_data + i) = ~*(m_data + i);
        for (uint16_t i = 0; i < end % m_block_size; ++i)
			*(m_data + end / m_block_size) ^= BlockType{ 1 } << i;
    }

    /**
     * Flip all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    void flip_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint64_t to_add = 1;
        if (begin % m_block_size)
        {
            for (uint16_t i = begin % m_block_size; i < m_block_size; ++i)
                *(m_data + begin / m_block_size) ^= BlockType{ 1 } << i;
        }
		else
			to_add = 0;

		for (uint64_t i = begin / m_block_size + to_add; i < end / m_block_size; ++i)
			*(m_data + i) = ~*(m_data + i);

        if (end % m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
				*(m_data + end / m_block_size) ^= BlockType{ 1 } << i;
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
            *(m_data + i / m_block_size) ^= BlockType{ 1 } << i % m_block_size;
        }
    }

    /**
     * Flips the block at the specified index
     * @param index Index of the block to flip (block index)
     */
    void flip_block(const uint64_t& index) noexcept
    {
	    *(m_data + index) = ~*(m_data + index);
    }

    /**
     * Flips all the blocks (same as flip())
     */
    void flip_block() noexcept
    {
        flip();
    }

    /**
     * Flips all the blocks in the specified range
     * @param end End of the range to fill (block index)
     */
    void flip_block_in_range(const uint64_t& end)
    {
        for (uint64_t i = 0; i < end; ++i)
            *(m_data + i) = ~*(m_data + i);
    }

    /**
     * Flips all the blocks in the specified range
     * @param begin Begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     */
    void flip_block_in_range(const uint64_t& begin, const uint64_t& end)
    {
        for (uint64_t i = begin; i < end; ++i)
            *(m_data + i) = ~*(m_data + i);
    }

    /**
     * Flips all the blocks in the specified range
     * @param begin Begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     * @param step Step size between the bits to flip (block step)
     */
    void flip_block_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step)
    {
        for (uint64_t i = begin; i < end; i += step)
            *(m_data + i) = ~*(m_data + i);
    }

    /**
     * Retrieves the value of a bit at a specified index
     * @param index The index of the bit to read (bit index)
     * @return The value of the bit at the specified index
     */
    [[nodiscard]] bool test(const uint64_t& index) const noexcept
    {
        return *(m_data + index / m_block_size) & BlockType{ 1 } << (m_block_size - index % m_block_size);
    }

    /**
     * Retrieves the block at the specified index
     * @param index Index of the block to retrieve (block index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] const BlockType& get_block(const uint64_t& index) const noexcept
    {
	    return *(m_data + index);
	}

    /**
     * Retrieves the block at the specified index
     * @param index Index of the block to retrieve (block index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] BlockType& get_block(const uint64_t& index) noexcept
    {
        return *(m_data + index);
    }


    /**
     * Checks if all bits are set 
     * @return true if all bits are set, false otherwise
     */
    [[nodiscard]] bool all() const noexcept
    {
        // check all except the last one if the size is not divisible by m_block_size
        for (BlockType* i = m_data; i < m_data + m_storage_size - (m_size % m_block_size ? 1 : 0); ++i)
        {
            if (*i != (std::numeric_limits<BlockType>::max)())
                return false;
        }
        if (m_size % m_block_size)
        {
            for (uint16_t i = 0; i < m_size % m_block_size; ++i)
            {
                if (!(*(m_data + m_size / m_block_size) & BlockType{ 1 } << i))
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
        for (BlockType* i = m_data; i < m_data + m_storage_size - (m_size % m_block_size ? 1 : 0); ++i)
        {
            if (*i)
                return true;
        }
        if (m_size % m_block_size)
		{
			for (uint16_t i = 0; i < m_size % m_block_size; ++i)
			{
				if (*(m_data + m_size / m_block_size) & BlockType{ 1 } << i)
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
        for (BlockType* i = m_data; i < m_data + m_storage_size; ++i)
        {
            if (*i)
                return false;
        }
        return true;
    }

    /**
     * Checks if all bits are reseted (none are set)
     * @return true if all bits are reseted, false otherwise
     */
    [[nodiscard]] bool all_reset() const noexcept
    {
        return none();
    }

    /**
     * @return The number of set bits
     */
    [[nodiscard]] uint64_t count() const noexcept
    {
        uint64_t count = 0;
        for (BlockType* i = m_data; i < m_data + m_storage_size; ++i)
        {
            BlockType j = *i;
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
		if (m_size % m_block_size)
		{
			if (value)
				*(m_data + m_size / m_block_size) |= BlockType{ 1 } << m_size % m_block_size;
			else
				*(m_data + m_size / m_block_size) &= ~(BlockType{ 1 } << m_size % m_block_size);
		}
		else
		{
			BlockType* new_data = new BlockType[m_storage_size + 1];
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
            if (!(m_size % m_block_size))
            {
                BlockType* new_data = new BlockType[m_storage_size - 1];
                std::copy(m_data, m_data + m_storage_size, new_data);
                delete[] m_data;
                m_data = new_data;
            }
            --m_size;
        }
        // else throw exception in safe version
    }

    /**
     * Pushes back a block to the Bitset, adjusting the size to the nearest multiple of sizeof(BlockType) upwards. [e.g. 65 bits -> (+8 {block} +7 {expanded area} = +15) -> 80 bits]
     * The bits in the expanded area may be initialized by previous calls, but their values are not explicitly defined by this function.
     * @param block The block to push back (block value)
     */
    void push_back_block(const BlockType& block) noexcept
    {
        BlockType* new_data = new BlockType[m_storage_size + (m_size % m_block_size ? 2 : 1)];
        if (m_data)
        {
            std::copy(m_data, m_data + m_storage_size, new_data);
            delete[] m_data;
        }
        m_data = new_data;
        *(m_data + m_storage_size++) = block;
        m_size += m_block_size + (m_size % m_block_size ? m_block_size - m_size % m_block_size : 0);
    }

    /**
     * Removes the last block from the bitset, adjusting the size to the nearest lower multiple of sizeof(BlockType). [e.g. 65 bits -> 64 bits -> 56 bits]
     */
    void pop_back_block() noexcept
    {
        if (m_data)
        {
            if (m_size % m_block_size)
                m_size = (m_storage_size - 1) * m_block_size;
            
            BlockType* new_data = new BlockType[m_storage_size - 1];
            std::copy(m_data, m_data + m_storage_size - 1, new_data);
            delete[] m_data;
            m_data = new_data;
            --m_storage_size;
            m_size -= m_block_size;
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
		BlockType* new_data = new BlockType[new_storage_size];
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
     * Returns the number of blocks the bitset would utilize for given size
     * @param size Size of the target bitset in bits
     * @return The number of blocks the bitset would utilize for the given size
     */
    [[nodiscard]] inline static constexpr uint64_t calculate_storage_size(const uint64_t& size) noexcept
    {
        return size / (sizeof(BlockType) * 8) + (size % (sizeof(BlockType) * 8) ? 1 : 0);
    }

	/**
	 * Creates a block of type T based on the given boolean value.
	 *
	 * @param value A boolean value indicating whether to create the block with the maximum value or zero.
	 * @return The created block of type T. If value is true, returns the maximum value representable by type T,
	 *         otherwise returns zero.
	 */
    [[nodiscard]] inline static constexpr BlockType create_filled_block(const bool& value)
    {
        return value ? (std::numeric_limits<BlockType>::max)() : 0u;
    }

	/**
	 * Underlying array of blocks containing the bits
	 */
	alignas(std::hardware_destructive_interference_size) BlockType* m_data;

	/**
	 * Size of the bitset in bits
	 */
	uint64_t m_size;

	/**
	 * Size of the bitset in blocks
	 */
	uint64_t m_storage_size;

	/**
	 * Bit-length of the underlying type
	 */
	static constexpr uint16_t m_block_size = sizeof(BlockType) * 8;
};

template <UnsignedInteger BlockType, uint64_t Size>
class CBitSet
{
public:
    /**
     * Empty constructor
     */
	constexpr CBitSet() noexcept : m_data() {}

    /**
     * Initializer list constructor
     * @param list Initializer list to fill the CBitSet with, must contain *blocks* not bits
     */
    constexpr CBitSet(const std::initializer_list<BlockType> list) noexcept
    {
        std::copy(list.begin(), list.end(), m_data);
    }

    /**
	 * Chunk value constructor
	 * @param block Chunk to fill the bitset with (block value)
	 */
    constexpr CBitSet(const BlockType& block) noexcept
	{
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = 0; i < m_storage_size; ++i)
                m_data[i] = block;
        }
        else
			std::fill(m_data, m_data + m_storage_size, block);
	}

    /**
     * Copy constructor
     * @param other Other CDynamicBitSet instance to copy from
     */
    constexpr CBitSet(const CBitSet& other) noexcept
    {
        std::copy(other.m_data, other.m_data + other.m_size / m_block_size + (other.m_size % m_block_size ? 1 : 0), m_data);
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
        return m_data[index / m_block_size] & BlockType{ 1 } << (m_block_size - index % m_block_size);
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
            m_data[index / m_block_size] |= BlockType{ 1 } << (m_block_size - index % m_block_size);
        else
            m_data[index / m_block_size] &= ~(BlockType{ 1 } << (m_block_size - index % m_block_size));
    }

    /**
     * Sets the bit at the specified index to 1 (true)
     * @param index Index of the bit to set (bit index)
     */
    constexpr void set(const uint64_t& index) noexcept
    {
        m_data[index / m_block_size] |= BlockType{1} << (m_block_size - index % m_block_size);
    }

    /**
     * Sets the bit at the specified index to 0 (false)
     * @param index Index of the bit to reset (bit index)
     */
    constexpr void reset(const uint64_t& index) noexcept
    {
        m_data[index / m_block_size] &= ~(BlockType{ 1 } << (m_block_size - index % m_block_size));
    }

    /**
     * Fills all the bits with the specified value
     * @param value Value to fill the bits with (bit value)
     */
    constexpr void fill(const bool& value) noexcept
    {
        if (std::is_constant_evaluated())
    	{
            for (BlockType& i : m_data)
                i = value ? (std::numeric_limits<BlockType>::max)() : 0;
        }
        else
        {
            ::memset(m_data, value ? 0xFF : 0, m_storage_size * sizeof(BlockType));
        }
    }

    /**
     * Clears all the bits (sets all bits to 0)
     */
    constexpr void reset() noexcept
    {
        if (std::is_constant_evaluated())
        {
			for (BlockType& i : m_data)
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
            for (BlockType& i : m_data)
                i = (std::numeric_limits<BlockType>::max)();
        }
        else
	        ::memset(m_data, 0xFF, m_storage_size * sizeof(BlockType));
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
            for (uint64_t i = 0; i < end / m_block_size; ++i)
                m_data[i] = value ? (std::numeric_limits<BlockType>::max)() : 0;
        }
        else
			::memset(m_data, value ? 255u : 0, end / m_block_size * sizeof(BlockType));
        if (end % m_block_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % m_block_size; ++i)
                    m_data[end / m_block_size] |= BlockType{ 1 } << i;
            }
            else if (!value)
            {
                for (uint16_t i = 0; i < end % m_block_size; ++i)
                    m_data[end / m_block_size] &= ~(BlockType{ 1 } << i);
            }
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param end End of the range to fill (bit index)
     */
    constexpr void reset_in_range(const uint64_t& end) noexcept
    {
        ::memset(m_data, 0, end / m_block_size * sizeof(BlockType));
        if (end % m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                m_data[end / m_block_size] &= ~(BlockType{ 1 } << i);
        }
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param end End of the range to fill (bit index)
     */
    constexpr void set_in_range(const uint64_t& end) noexcept
    {
        ::memset(m_data, (std::numeric_limits<BlockType>::max)(), end / m_block_size * sizeof(BlockType));
        if (end % m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                m_data[end / m_block_size] |= BlockType{ 1 } << i;
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
        constexpr BlockType max_value = (std::numeric_limits<BlockType>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_block and fill the first byte with it
        if (begin % m_block_size)
        {
            const uint16_t end_bit = (begin / m_block_size == end / m_block_size) ? end % m_block_size : m_block_size;
            if (value)
            {
                for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                    m_data[begin / m_block_size] |= BlockType{ 1 } << i;
            }
            else
            {
                for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                    m_data[begin / m_block_size] &= ~(BlockType{ 1 } << i);
            }
        }
        else
            to_add = 0;

        // set the end block if the end is not aligned with the block size
        if (end % m_block_size && begin / m_block_size != end / m_block_size)
        {
            if (value)
            {
                for (uint16_t i = 0; i < end % m_block_size; ++i)
                    m_data[end / m_block_size] |= BlockType{ 1 } << i;
            }
            else
            {
                for (uint16_t i = 0; i < end % m_block_size; ++i)
                    m_data[end / m_block_size] &= ~(BlockType{ 1 } << i);
            }
        }
        else
            to_sub = 0;
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = begin / m_block_size + to_add; i < (end - begin) / m_block_size * sizeof(BlockType) - to_sub; ++i)
                m_data[i] = value ? max_value : 0;
        }
        else
			::memset(m_data + begin / m_block_size + to_add, value ? max_value : 0, (end - begin) / m_block_size * sizeof(BlockType) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void reset_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint8_t to_add = 1, to_sub = 1;
        // create begin_block and fill the first byte with it
        if (begin % m_block_size)
        {
            const uint16_t end_bit = begin / m_block_size == end / m_block_size ? end % m_block_size : m_block_size;
            for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                m_data[end / m_block_size] &= ~(BlockType{ 1 } << i);
        }
        else
            to_add = 0;

        // set the end block if the end is not aligned with the block size
        if (end % m_block_size && begin / m_block_size != end / m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                m_data[end / m_block_size] &= ~(BlockType{ 1 } << i);
        }
        else
            to_sub = 0;
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = begin / m_block_size + to_add; i < (end - begin) / m_block_size * sizeof(BlockType) - to_sub; ++i)
                m_data[i] = 0;
        }
        else
			::memset(m_data + begin / m_block_size + to_add, 0, (end - begin) / m_block_size * sizeof(BlockType) - to_sub);
    }

    /**
     * Fills all the bits in the specified range with 1 (true)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void set_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        constexpr BlockType max_value = (std::numeric_limits<BlockType>::max)();
        uint8_t to_add = 1, to_sub = 1;
        // create begin_block and fill the first byte with it
        if (begin % m_block_size)
        {
            const uint16_t end_bit = (begin / m_block_size == end / m_block_size) ? end % m_block_size : m_block_size;
            for (uint16_t i = begin % m_block_size; i < end_bit; ++i)
                m_data[begin / m_block_size] |= BlockType{ 1 } << i;
        }
        else
            to_add = 0;

        // set the end block if the end is not aligned with the block size
        if (end % m_block_size && begin / m_block_size != end / m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                m_data[end / m_block_size] |= BlockType{ 1 } << i;
        }
        else
            to_sub = 0;
        if (std::is_constant_evaluated())
        {
            for (uint64_t i = begin / m_block_size + to_add; i < (end - begin) / m_block_size * sizeof(BlockType) - to_sub; ++i)
                m_data[i] = max_value;
        }
        else 
            ::memset(m_data + begin / m_block_size + to_add, max_value, (end - begin) / m_block_size * sizeof(BlockType) - to_sub);
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
                m_data[i / m_block_size] |= BlockType{ 1 } << i % m_block_size;
            else
                m_data[i / m_block_size] &= ~(BlockType{ 1 } << i % m_block_size);
        }
    }

    /**
     * Fills all the bits in the specified range with 0 (false)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    constexpr void reset_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i / m_block_size] &= ~(BlockType{ 1 } << i % m_block_size);
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
            m_data[i / m_block_size] |= BlockType{ 1 } << i % m_block_size;
    }

    /**
     * !!! W.I.P. - Does not function correctly at the moment !!!\n
     * Fill the bits in the specified range with the specified value using an optimized algorithm.\n
     * This algorithm is particularly efficient when the step size is relatively low.\n
     * Note: This function has a rather complex implementation. It is not recommended to use it when simple filling without a step is possible.\n
     * Performance of this function varies significantly depending on the step. It performs best when step is a multiple of m_block_size, and is within reasonable range from it.\n
     * However, worst when step is not aligned with m_block_size and end is not aligned with m_block_size. In such cases, extra processing is required to handle the boundary blocks.\n
     * @param value Value to fill the bits with (bit value)
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     * @param step Step size between the bits to fill (bit step)
     */
    constexpr void fill_in_range_optimized(const bool& value, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        // Initialize variables
        uint64_t blocks_size, current_block = begin / m_block_size + 1 + step / m_block_size, current_offset = 0;
        uint16_t offset;
        const uint64_t end_block = end / m_block_size + (end % m_block_size ? 1 : 0);

        // Determine the size of blocks based on step and block size
        if ((step % 2 || step <= m_block_size) && m_block_size % step) {
            blocks_size = (std::min)(m_storage_size, step);
        }
        else if (!(m_block_size % step))
            blocks_size = 1;
        else if (!(step % m_block_size))
            blocks_size = step / m_block_size;
        else
        {
            // GCD of step and m_block_size
            if (step % m_block_size)
            {
                uint64_t a = step, b = m_block_size, t = m_block_size;
                while (b) {
                    t = b;
                    b = a % b;
                    a = t;
                }
                blocks_size = a;
            }
            else
                blocks_size = 1;
        }

        // Calculate the offset
        if (begin < m_block_size)
        {
            offset = (m_block_size - begin) % step;
            if (offset)
                offset = step - offset;
        }
        else
        {
            offset = (begin - m_block_size) % step;
        }

        if (offset)
            offset = (m_block_size - offset + step / m_block_size * m_block_size) % step;

        std::cout << "offset: " << offset << '\n';

        // Create and apply the beginning block
        {
            const uint16_t end_bit = (begin / m_block_size == end / m_block_size) ? end % m_block_size : m_block_size;
            if (value)
            {
                for (uint16_t i = begin % m_block_size; i < end_bit; i += step)
                    *(m_data + begin / m_block_size) |= BlockType{ 1 } << i;
            }
            else
            {
                for (uint16_t i = begin % m_block_size; i < end_bit; i += step)
                    *(m_data + begin / m_block_size) &= ~(BlockType{ 1 } << i);
            }
        }


        // Fill with appropriate block
        std::cout << blocks_size << " blocks\n";
        std::cout << (std::min)(blocks_size + begin / m_block_size, m_storage_size) << " blocks\n";
        for (uint64_t i = 0; i < (std::min)(blocks_size, m_storage_size); ++i)
        {
            // Generate block for the current iteration
            BlockType block = 0;

            if (value)
            {
                for (uint16_t j = !i ? offset : 0; j < m_block_size; ++j)
                {
                    std::cout << current_block * m_block_size + j - offset << '\n';
                    if (!((current_block * m_block_size + j - offset) % step))
                        block |= BlockType{ 1 } << j;
                }
            }
            else
            {
                block = (std::numeric_limits<BlockType>::max)();
                for (uint16_t j = (!i ? offset : 0); j < m_block_size; ++j)
                {
                    if (!((current_block * m_block_size + j - offset) % step))
                        block &= ~(BlockType{ 1 } << j);
                }
            }

            // print the block
            for (uint16_t j = 0; j < m_block_size; ++j)
            {
                std::cout << ((block & BlockType{ 1 } << j) >> j);
            }

            std::cout << '\n';

            // Apply the block
            for (uint64_t j = current_block; j < m_storage_size; ++j)
            {
                if (j == end_block - 1 && end % m_block_size)
                {
                    // Remove bits that overflow the range
                    if (value)
                    {
                        for (uint16_t k = end % m_block_size; k < m_block_size; ++k)
                            block &= ~(BlockType{ 1 } << k);
                        *(m_data + j) |= block;
                    }
                    else
                    {
                        for (uint16_t k = end % m_block_size; k < m_block_size; ++k)
                            block |= BlockType{ 1 } << k;
                        *(m_data + j) &= block;
                    }
                    break;
                }
                if (value)
                    *(m_data + j) |= block;
                else
                    *(m_data + j) &= block;
            }
            ++current_block;
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
        if (step <= m_block_size * 2.5) // approximately up until this point it is faster, though no scientific anything went into this, just a guess lol
        {
            fill_in_range_optimized(value, begin, end, step);
            return;
        }
        set_in_range(value, begin, end, step);
    }

    /**
     * Sets the block at the specified index to the specified value
     * @param block Chunk to set (block value)
     * @param index Index of the block to set (block index)
     */
    constexpr void set_block(const BlockType& block, const uint64_t& index) noexcept
    {
        m_data[index] = block;
    }

    /**
     * Fills all the blocks with the specified block
     * @param block Chunk to fill the blocks with (block value)
     */
    constexpr void fill_block(const BlockType& block) noexcept
    {
        for (uint64_t i = 0; i < m_storage_size; ++i)
            m_data[i] = block;
    }

    /**
     * Fills all the bits in the specified range with the specified block
     * @param block Chunk to fill the bits with (block value)
     * @param end End of the range to fill (block index)
     */
    constexpr void fill_block_in_range(const BlockType& block, const uint64_t& end) noexcept
    {
        for (uint64_t i = 0; i < end; ++i)
            m_data[i] = block;
    }

    /**
     * Fills all the bits in the specified range with the specified block
     * @param block Chunk to fill the bits with
     * @param begin begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     */
    constexpr void fill_block_in_range(const BlockType& block, const uint64_t& begin, const uint64_t& end) noexcept
    {
        for (uint64_t i = begin; i < end; ++i)
            m_data[i] = block;
    }

    /**
     * Fills all the bits in the specified range with the specified block
     * @param block Chunk to fill the bits with (block value)
     * @param begin begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     * @param step Step size between the bits to fill (block step)
     */
    constexpr void fill_block_in_range(const BlockType& block, const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i] = block;
    }

    /**
     * Flips the bit at the specified index
     * @param index Index of the bit to flip (bit index)
     */
    constexpr void flip(const uint64_t& index) noexcept
    {
        m_data[index / m_block_size] ^= BlockType{ 1 } << (m_block_size - index % m_block_size);
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
        // flip blocks that are in range by bulk, rest flip normally
        for (uint64_t i = 0; i < end / m_block_size; ++i)
            m_data[i] = ~m_data[i];
        for (uint16_t i = 0; i < end % m_block_size; ++i)
            m_data[end / m_block_size] ^= BlockType{ 1 } << i;
    }

    /**
     * Flip all the bits in the specified range
     * @param begin Begin of the range to fill (bit index)
     * @param end End of the range to fill (bit index)
     */
    constexpr void flip_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        uint64_t to_add = 1;
        if (begin % m_block_size)
        {
            for (uint16_t i = begin % m_block_size; i < m_block_size; ++i)
                m_data[begin / m_block_size] ^= BlockType{ 1 } << i;
        }
        else
            to_add = 0;

        for (uint64_t i = begin / m_block_size + to_add; i < end / m_block_size; ++i)
            m_data[i] = ~m_data[i];

        if (end % m_block_size)
        {
            for (uint16_t i = 0; i < end % m_block_size; ++i)
                m_data[end / m_block_size] ^= BlockType{ 1 } << i;
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
            m_data[i] ^= BlockType{ 1 } << i % m_block_size;
    }

    /**
     * Flips the block at the specified index
     * @param index Index of the block to flip (block index)
     */
    constexpr void flip_block(const uint64_t& index) noexcept
    {
        m_data[index] = ~m_data[index];
    }

    /**
     * Flips all the blocks (same as flip())
     */
    constexpr void flip_block() noexcept
    {
        flip();
    }

    /**
     * Flips all the blocks in the specified range
     * @param end End of the range to fill (block index)
     */
    constexpr void flip_block_in_range(const uint64_t& end) noexcept
    {
        for (uint64_t i = 0; i < end; ++i)
            m_data[i] = ~m_data[i];
    }

    /**
     * Flips all the blocks in the specified range
     * @param begin Begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     */
    constexpr void flip_block_in_range(const uint64_t& begin, const uint64_t& end) noexcept
    {
        for (uint64_t i = begin; i < end; ++i)
            m_data[i] = ~m_data[i];
    }

    /**
     * Flips all the blocks in the specified range
     * @param begin Begin of the range to fill (block index)
     * @param end End of the range to fill (block index)
     * @param step Step size between the bits to flip (block step)
     */
    constexpr void flip_block_in_range(const uint64_t& begin, const uint64_t& end, const uint64_t& step) noexcept
    {
        for (uint64_t i = begin; i < end; i += step)
            m_data[i] = ~m_data[i];
    }

    /**
     * Retrieves the value of a bit at a specified index
     * @param index The index of the bit to read (bit index)
     * @return The value of the bit at the specified index
     */
    [[nodiscard]] constexpr bool test(const uint64_t& index) const noexcept
    {
        return m_data[index / m_block_size] & BlockType{ 1 } << (m_block_size - index % m_block_size);
    }

    /**
     * Retrieves the block at the specified index
     * @param index Index of the block to retrieve (block index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] constexpr const BlockType& get_block(const uint64_t& index) const noexcept
    {
        return m_data[index];
    }

    /**
     * Retrieves the block at the specified index
     * @param index Index of the block to retrieve (block index)
     * @return Chunk at the specified index
     */
    [[nodiscard]] constexpr BlockType& get_block(const uint64_t& index) noexcept
    {
        return m_data[index];
    }


    /**
     * Checks if all bits are set
     * @return true if all bits are set, false otherwise
     */
    [[nodiscard]] constexpr bool all() const noexcept
    {
        // check all except the last one if the size is not divisible by m_block_size
        for (uint64_t i = 0; i < m_storage_size - (m_size % m_block_size ? 1 : 0); ++i)
        {
	        if (m_data[i] != (std::numeric_limits<BlockType>::max)())
				return false;
		}
        if (m_size % m_block_size)
        {
            for (uint16_t i = 0; i < m_size % m_block_size; ++i)
            {
                if (!(m_data[m_size / m_block_size] & BlockType{ 1 } << i))
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
		for (uint64_t i = 0; i < m_storage_size - (m_size % m_block_size ? 1 : 0); ++i)
		{
			if (m_data[i])
				return true;
		}
		if (m_size % m_block_size)
		{
			for (uint16_t i = 0; i < size % m_block_size; ++i)
			{
				if (m_data[m_size / m_block_size] & BlockType{ 1 } << i)
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
		for (uint64_t i = 0; i < m_storage_size - (m_size % m_block_size ? 1 : 0); ++i)
		{
			if (m_data[i])
				return false;
		}
        if (m_size % m_block_size)
        {
            for (uint16_t i = 0; i < m_size % m_block_size; ++i)
            {
                if (m_data[m_size / m_block_size] & BlockType{ 1 } << i)
                    return true;
            }
        }
        return false;
    }


    /**
     * Checks if all bits are reseted (none are set)
     * @return true if all bits are reseted, false otherwise
     */
    [[nodiscard]] constexpr bool all_reset() const noexcept
    {
        return none();
    }

    /**
     * @return The number of set bits
     */
    [[nodiscard]] constexpr uint64_t count() const noexcept
    {
        uint64_t count = 0;
        for (BlockType* i = m_data; i < m_data + m_storage_size; ++i)
        {
            BlockType j = *i;
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
     * Returns the number of blocks the Bitset would utilize for given size
     * @param size Size of the target Bitset in bits
     * @return The number of blocks the Bitset would utilize for the given size
     */
    [[nodiscard]] inline static constexpr uint64_t calculate_storage_size(const uint64_t& size) noexcept
    {
        return m_size / (sizeof(BlockType) * 8) + (size % (sizeof(BlockType) * 8) ? 1 : 0);
    }

    /**
     * Creates a block of type T based on the given boolean value.
     *
     * @param value A boolean value indicating whether to create the block with the maximum value or zero.
     * @return The created block of type T. If value is true, returns the maximum value representable by type T,
     *         otherwise returns zero.
     */

    [[nodiscard]] inline static constexpr BlockType create_filled_block(const bool& value)
    {
        return value ? (std::numeric_limits<BlockType>::max)() : 0u;
    }

    /**
     * Size of the Bitset in bits
     */
    static constexpr uint64_t m_size = Size;

    /**
     * Size of the Bitset in blocks
     */
    static constexpr uint64_t m_storage_size = calculate_storage_size(m_size);

    /**
     * Bit-length of the underlying type
     */
    static constexpr uint16_t m_block_size = sizeof(BlockType) * 8;

    /**
	 * Underlying array of blocks containing the bits
	 */
    alignas(std::hardware_destructive_interference_size) BlockType m_data[m_storage_size] = { 0 };
};