#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define the structure for a bitset
typedef struct 
{
    uint8_t* data;
    uint64_t size, storage_size;
} BitSet;

// Function prototypes
inline void bitset_init(BitSet* const bitset, const uint64_t size);
inline void bitset_init_with_value(BitSet* const bitset, const uint64_t size, const uint64_t value);
inline void bitset_destroy(const BitSet* const bitset);
inline void bitset_copy_from(BitSet* const destination, const BitSet* const source);
inline void bitset_move_from(BitSet* const destination, BitSet* const source);
inline bool bitset_get_bit(const BitSet* const bitset, const uint64_t index);
inline void bitset_set_bit_value(BitSet* const bitset, const uint64_t value, const uint64_t index);
inline void bitset_set_bit(BitSet* const bitset, const uint64_t index); 
inline void bitset_clear_bit(BitSet* const bitset, const uint64_t index);
inline void bitset_fill_all(BitSet* const bitset, const bool value);
inline void bitset_clear_all(BitSet* const bitset);
inline void bitset_set_all(BitSet* const bitset);
inline void bitset_fill_in_range_end(BitSet* const bitset, const bool value, const uint64_t end);
inline void bitset_clear_in_range_end(BitSet* const bitset, const uint64_t end);
inline void bitset_set_in_range_end(BitSet* const bitset, const uint64_t end);
inline void bitset_fill_in_range_begin_end(BitSet* const bitset, const bool value, const uint64_t begin, const uint64_t end);
inline void bitset_clear_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end);
inline void bitset_set_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end);
inline void bitset_fill_in_range_begin_end_step(BitSet* const bitset, const bool value, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_clear_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_set_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_set_chunk(BitSet* const bitset, const uint8_t chunk, const uint64_t index);
inline void bitset_fill_all_chunks(BitSet* const bitset, const uint8_t value);
inline void bitset_fill_chunk_in_range_end(BitSet* const bitset, const uint8_t chunk, const uint64_t end);
inline void bitset_fill_chunk_in_range_begin_end(BitSet* const bitset, const uint8_t chunk, const uint64_t begin, const uint64_t end);
inline void bitset_fill_chunk_in_range_begin_end_step(BitSet* const bitset, const uint8_t chunk, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_flip_bit(BitSet* const bitset, const uint64_t index);
inline void bitset_flip_all(BitSet* const bitset);
inline void bitset_flip_in_range_end(BitSet* const bitset, const uint64_t end);
inline void bitset_flip_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end);
inline void bitset_flip_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_flip_chunk(BitSet* const bitset, const uint64_t index);
inline void bitset_flip_all_chunks(BitSet* const bitset);
inline void bitset_flip_chunk_in_range_end(BitSet* const bitset, const uint64_t end);
inline void bitset_flip_chunk_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end);
inline void bitset_flip_chunk_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step);
inline uint8_t bitset_get_chunk(const BitSet* const bitset, const uint64_t index);
inline bool bitset_all_set(const BitSet* const bitset);
inline bool bitset_any_set(const BitSet* const bitset);
inline bool bitset_none_set(const BitSet* const bitset);
inline bool bitset_all_cleared(const BitSet* const bitset);
inline uint64_t bitset_count_set(const BitSet* const bitset);
inline bool bitset_empty(const BitSet* const bitset);
inline uint64_t bitset_calculate_storage_size(const uint64_t size);

/**
 * Size initialization
 * @param bitset The bitset to initialize
 * @param size The size of the bitset to be initialized
 */
inline void bitset_init(BitSet* const bitset, const uint64_t size) 
{
    bitset->size = size;
    bitset->storage_size = size / (sizeof(uint8_t) * 8) + (size % (sizeof(uint8_t) * 8) ? 1 : 0);
    bitset->data = (uint8_t*)malloc(bitset->storage_size * sizeof(uint8_t));
    bitset_fill_all(bitset, 0);
}

/**
 * Size and value initialization
 * @param bitset The bitset to initialize
 * @param size The size of the bitset to be initialized
 * @param value The value to fill the bitset with
 */
inline void bitset_init_with_value(BitSet* const bitset, const uint64_t size, const uint64_t value) 
{
    bitset->size = size;
    bitset->storage_size = size / (sizeof(uint8_t) * 8) + (size % (sizeof(uint8_t) * 8) ? 1 : 0);
    bitset->data = (uint8_t*)malloc(bitset->storage_size * sizeof(uint8_t));
    bitset_fill_all(bitset, value);
}

/**
 * Destroys the bitset (frees the memory)
 * @param bitset The bitset to destroy
 */
inline void bitset_destroy(const BitSet* const bitset) 
{
    free(bitset->data);
}

/**
 * Copies the data from one bitset to another
 * @param destination The bitset to copy to
 * @param source The bitset to copy from
 */
inline void bitset_copy_from(BitSet* const destination, const BitSet* const source) 
{
    for (uint64_t i = 0; i < destination->size; ++i)
        *(destination->data + i) = *(source->data + i);
}

/**
 * Moves the data from one bitset to another
 * @param destination The bitset to move to
 * @param source The bitset to move from
 */
inline void bitset_move_from(BitSet* const destination, BitSet* source) 
{
    destination->size = source->size;
    source->size = 0;
    destination->data = source->data;
    source->data = NULL;
}

/**
 * Sets the value of a bit at a specified index
 * @param bitset The bitset to modify
 * @param value The value to set the bit to
 * @param index The index of the bit to modify (bit index)
 */
inline void bitset_set_bit_value(const BitSet* const bitset, const uint64_t value, const uint64_t index) 
{
    if (value)
        *(bitset->data + index / sizeof(uint8_t)) |= 1 << index % sizeof(uint8_t);
    else
        *(bitset->data + index / sizeof(uint8_t)) &= ~(1 << index % sizeof(uint8_t));
}

/**
 * Retrieves the value of a bit at a specified index
 * @param bitset The bitset to read from
 * @param index The index of the bit to read (bit index)
 * @return The value of the bit at the specified index
 */
inline bool bitset_get_bit(const BitSet* const bitset, const uint64_t index) {
    return (*(bitset->data + index / (sizeof(uint8_t) * 8)) & 1 << index % (sizeof(uint8_t) * 8)) >> index % (sizeof(uint8_t) * 8);
}

/**
 * Sets the value of a bit at a specified index to 1 (true)
 * @param bitset The bitset to modify
 * @param index The index of the bit to set (bit index)
 */
inline void bitset_set_bit(BitSet* const bitset, const uint64_t index) {
    *(bitset->data + index / sizeof(uint8_t)) |= 1 << index % sizeof(uint8_t);
}

/**
 * Sets the value of a bit at a specified index to 0 (false)
 * @param bitset The bitset to modify
 * @param index The index of the bit to clear (bit index)
 */
inline void bitset_clear_bit(BitSet* const bitset, const uint64_t index) {
    *(bitset->data + index / sizeof(uint8_t)) &= ~(1 << index % sizeof(uint8_t));
}

/**
 * Fills the bitset with a specified value
 * @param bitset The bitset to modify
 * @param value The value to fill the bitset with
 */
inline void bitset_fill_all(BitSet* const bitset, const bool value) {
    const uint8_t chunk = value ? 255u : 0u;
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
        *(bitset->data + i) = chunk;
}

/**
 * Clears all the bits (sets all bits to 0)
 * @param bitset The bitset to modify
 */
inline void bitset_clear_all(BitSet* const bitset) {
    memset(bitset->data, 0, bitset->storage_size);
}

/**
 * Sets all the bits (sets all bits to 1)
 * @param bitset The bitset to modify
 */
inline void bitset_set_all(BitSet* const bitset) {
    memset(bitset->data, 255, bitset->storage_size);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset The bitset to modify
 * @param value The value to fill the bits with (bit value)
 * @param end End of the range to fill (bit index)
 */
inline void bitset_fill_in_range_end(BitSet* const bitset, const bool value, const uint64_t end) {
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = value ? 255u : 0u;
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bitset The bitset to modify
 * @param end End of the range to fill (bit index)
 */
inline void bitset_clear_in_range(BitSet* const bitset, const uint64_t end)
{
    memset(bitset->data, 0, end / 8u);
    if (end % 8u)
    {
        for (uint16_t i = 0; i < end % 8u; ++i)
            *(bitset->data + end / 8u) &= ~(1u << i);
    }
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset The bitset to modify
 * @param end End of the range to fill (bit index)
 */
inline void bitset_set_in_range(BitSet* const bitset, const uint64_t end)
{
    memset(bitset->data, 255u, end / 8u);
    if (end % 8u)
    {
        for (uint16_t i = 0; i < end % 8u; ++i)
            *(bitset->data + end / 8u) |= 1u << i;
    }
}

/**
 * Fills all the bits in the specified range with the specified value
 * @param bitset The bitset to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 */
inline void bitset_fill_in_range_begin_end(BitSet* const bitset, const bool value, const uint64_t begin, const uint64_t end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_chunk and fill the first byte with it
    if (begin % 8)
    {
        const uint16_t end_bit = (begin / 8 == end / 8) ? end % 8 : 8;
        if (value)
        {
            for (uint16_t i = begin % 8; i < end_bit; ++i)
                *(bitset->data + begin / 8) |= 1 << i;
        }
        else
        {
            for (uint16_t i = begin % 8; i < end_bit; ++i)
                *(bitset->data + begin / 8) &= ~(1 << i);
        }
    }
    else
        to_add = 0;
    if (end % 8 && begin / 8 != end / 8)
    {
        if (value)
        {
            for (uint16_t i = 0; i < end % 8; ++i)
                *(bitset->data + end / 8) |= 1u << i;
        }
        else
        {
            for (uint16_t i = 0; i < end % 8; ++i)
                *(bitset->data + end / 8) &= ~(1u << i);
        }
    }
    else
        to_sub = 0;

    memset(bitset->data + begin / 8 + to_add, value ? 255u : 0u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bitset The bitset to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 */
inline void bitset_clear_in_range_begin_end(BitSet* const bitset, const uint64_t& begin, const uint64_t& end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_chunk and fill the first byte with it
    if (begin % 8u)
    {
        const uint16_t end_bit = begin / 8u == end / 8u ? end % 8u : 8u;
        for (uint16_t i = begin % 8u; i < end_bit; ++i)
            *(bitset->data + begin / 8u) &= ~(1u << i);
    }
    else
        to_add = 0;

    // set the end chunk if the end is not aligned with the chunk size
    if (end % 8 && begin / 8 != end / 8)
    {
        for (uint16_t i = 0; i < end % 8; ++i)
            *(bitset->data + end / 8) &= ~(1u << i);
    }
    else
        to_sub = 0;

    memset(bitset->data + begin / 8u + to_add, 0u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset The bitset to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 */
inline void bitset_set_in_range_begin_end(BitSet* const bitset, const uint64_t& begin, const uint64_t& end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_chunk and fill the first byte with it
    if (begin % 8u)
    {
        const uint16_t end_bit = begin / 8u == end / 8u ? end % 8u : 8u;
        for (uint16_t i = begin % 8u; i < end_bit; ++i)
            *(bitset->data + begin / 8u) |= 1u << i;
    }
    else
        to_add = 0;

    // set the end chunk if the end is not aligned with the chunk size
    if (end % 8 && begin / 8 != end / 8)
    {
        for (uint16_t i = 0; i < end % 8; ++i)
            *(bitset->data + end / 8) |= 1u << i;
    }
    else
        to_sub = 0;

    memset(bitset->data + begin / 8u + to_add, 255u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with the specified value
 * @param bitset The bitset to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 */
inline void bitset_fill_in_range_begin_end_step(BitSet* const bitset, const bool value, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
    {
        if (value)
            *(bitset->data + i / sizeof(uint8_t)) |= 1 << i % sizeof(uint8_t);
        else
            *(bitset->data + i / sizeof(uint8_t)) &= ~(1 << i % sizeof(uint8_t));
    }
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bitset The bitset to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 */
inline void bitset_clear_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i / sizeof(uint8_t)) &= ~(1 << i % sizeof(uint8_t));
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset The bitset to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 */
inline void bitset_set_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i / sizeof(uint8_t)) |= 1 << i % sizeof(uint8_t);
}

/**
 * Sets the chunk at the specified index to the specified value
 * @param bitset The bitset to modify
 * @param chunk The chunk to set (chunk value)
 * @param index The index of the chunk to set (chunk index)
 */
inline void bitset_set_chunk(BitSet* const bitset, const uint8_t chunk, const uint64_t index)
{
    *(bitset->data + index) = chunk;
}

/**
 * Fills all the chunks in the specified range with the specified value
 * @param bitset The bitset to modify
 * @param value The value to fill the bits with (chunk value)
 */
inline void bitset_fill_all_chunks(BitSet* const bitset, const uint8_t value)
{
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
        *(bitset->data + i) = value;
}

/**
 * Fills all the bits in the specified range with the specified chunk
 * @param bitset The bitset to modify
 * @param chunk Chunk to fill the bits with (chunk value)
 * @param end End of the range to fill (chunk index)
 */
inline void bitset_fill_chunk_in_range_end(BitSet* const bitset, const uint8_t chunk, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = chunk;
}

/**
 * Fills all the bits in the specified range with the specified chunk
 * @param bitset The bitset to modify
 * @param chunk Chunk to fill the bits with
 * @param begin begin of the range to fill (chunk index)
 * @param end End of the range to fill (chunk index)
 */
inline void bitset_fill_chunk_in_range_begin_end(BitSet* const bitset, const uint8_t chunk, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bitset->data + i) = chunk;
}

/**
 * Fills all the bits in the specified range with the specified chunk
 * @param bitset The bitset to modify
 * @param chunk Chunk to fill the bits with (chunk value)
 * @param begin begin of the range to fill (chunk index)
 * @param end End of the range to fill (chunk index)
 * @param step Step size between the bits to fill (chunk step)
 */
inline void bitset_fill_chunk_in_range_begin_end_step(BitSet* const bitset, const uint8_t chunk, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i) = chunk;
}

/**
 * Flips the bit at the specified index
 * @param bitset The bitset to modify
 * @param index Index of the bit to flip (bit index)
 */
inline void bitset_flip_bit(BitSet* const bitset, const uint64_t index)
{
    *(bitset->data + index / sizeof(uint8_t)) ^= 1u << index % sizeof(uint8_t);
}

/**
 * Flips all the bits
 * @param bitset The bitset to modify
 */
inline void bitset_flip_all(BitSet* const bitset)
{
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bitset The bitset to modify
 * @param end End of the range to flip (bit index)
 */
inline void bitset_flip_in_range_end(BitSet* const bitset, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bitset The bitset to modify
 * @param begin Begin of the range to flip (bit index)
 * @param end End of the range to flip (bit index)
 */
inline void bitset_flip_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bitset The bitset to modify
 * @param begin Begin of the range to flip (bit index)
 * @param end End of the range to flip (bit index)
 * @param step Step size between the bits to flip (bit step)
 */
inline void bitset_flip_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i / sizeof(uint8_t)) ^= 1u << i % sizeof(uint8_t);
}

/**
 * Flips the chunk at the specified index
 * @param bitset The bitset to modify
 * @param index Index of the chunk to flip (chunk index)
 */
inline void bitset_flip_chunk(BitSet* const bitset, const uint64_t index)
{
    *(bitset->data + index) = ~*(bitset->data + index);
}

/**
 * Flips all the chunks (same as bitset_flip_all in practice)
 * @param bitset The bitset to modify
 */
inline void bitset_flip_all_chunks(BitSet* const bitset)
{
    bitset_flip_all(bitset);
}

/**
 * Flips all the chunks in the specified range
 * @param bitset The bitset to modify
 * @param end End of the range to flip (chunk index)
 */
inline void bitset_flip_chunk_in_range_end(BitSet* const bitset, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the chunks in the specified range
 * @param bitset The bitset to modify
 * @param begin Begin of the range to flip (chunk index)
 * @param end End of the range to flip (chunk index)
 */
inline void bitset_flip_chunk_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the chunks in the specified range
 * @param bitset The bitset to modify
 * @param begin Begin of the range to flip (chunk index)
 * @param end End of the range to flip (chunk index)
 * @param step Step size between the chunks to flip (chunk step)
 */
inline void bitset_flip_chunk_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Retrieves the chunk at the specified index
 * @param bitset The bitset to read from
 * @param index Index of the chunk to read (chunk index)
 * @return The chunk at the specified index
 */
inline uint8_t bitset_get_chunk(const BitSet* const bitset, const uint64_t index)
{
    return *(bitset->data + index);
}

/**
 * Checks if all the bits are set
 * @param bitset The bitset to check
 * @return True if all the bits are set, false otherwise
 */
inline bool bitset_all_set(const BitSet* const bitset)
{
    for (uint64_t i = 0; i < bitset->storage_size - (bitset->size % 8 ? 1 : 0); ++i)
    {
        if (*(bitset->data + i) != 255u)
            return false;
    }
    if (bitset->size % 8)
    {
        for (uint16_t i = 0; i < bitset->size % 8; ++i)
        {
            if (*(bitset->data + bitset->storage_size - 1) & 1u << i)
                return false;
        }
    }
    return true;
}

/**
 * Checks if any of the bits are set
 * @param bitset The bitset to check
 * @return True if any of the bits are set, false otherwise
 */
inline bool bitset_any_set(const BitSet* const bitset)
{
    for (uint64_t i = 0; i < bitset->storage_size - (bitset->size % 8 ? 1 : 0); ++i)
    {
        if (*(bitset->data + i))
            return true;
    }
    if (bitset->size % 8)
    {
        for (uint16_t i = 0; i < bitset->size % 8; ++i)
        {
            if (*(bitset->data + bitset->storage_size - 1) & 1u << i)
                return true;
        }
    }
    return false;
}

/**
 * Checks if none of the bits are set
 * @param bitset The bitset to check
 * @return True if none of the bits are set, false otherwise
 */
inline bool bitset_none_set(const BitSet* const bitset)
{
    return bitset_all_cleared(bitset);
}

/**
 * Checks if all the bits are clear
 * @param bitset The bitset to check
 * @return True if all the bits are clear, false otherwise
 */
inline bool bitset_all_cleared(const BitSet* const bitset)
{
    for (uint64_t i = 0; i < bitset->storage_size - (bitset->size % 8u ? 1 : 0); ++i)
    {
        if (*(bitset->data + i))
            return false;
    }
    if (bitset->size % 8)
    {
        for (uint16_t i = 0; i < bitset->size % 8; ++i)
        {
            if (*(bitset->data + bitset->storage_size - 1) & 1u << i)
                return false;
        }
    }
    return true;
}

/**
 * @return the number of bits set in the bitset
 */
inline uint64_t bitset_count_set(const BitSet* const bitset)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
    {
        uint8_t chunk = *(bitset->data + i);
        while (chunk)
        {
            count += chunk & 1u;
            chunk >>= 1;
        }
    }
    return count;
}

/**
 * Check if bitset is empty
 * @param bitset The bitset to check
 * @return True if the bitset is empty, false otherwise
*/
inline bool bitset_empty(const BitSet* const bitset)
{
    return !bitset->size;
}

/**
 * Calculates the number of bytes required to store the bitset
 * @param size The size of the bitset
 * @return The number of bytes required to store the bitset
 */
inline uint64_t bitset_calculate_storage_size(const uint64_t size)
{
    return size / (sizeof(uint8_t) * 8) + (size % (sizeof(uint8_t) * 8) ? 1 : 0);
}