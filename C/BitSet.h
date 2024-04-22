#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define the structure for a bit_set
typedef struct 
{
    uint8_t* data;
    uint64_t size, storage_size;
} BitSet;

inline void bit_set_init(BitSet* const bit_set, const uint64_t size);
inline void bit_set_init_with_value(BitSet* const bit_set, const uint64_t size, const uint64_t value);
inline void bit_set_destroy(const BitSet* const bit_set);
inline void bit_set_copy_from(BitSet* const destination, const BitSet* const source);
inline void bit_set_move_from(BitSet* const destination, BitSet* const source);
inline bool bit_set_get(const BitSet* const bit_set, const uint64_t index);
inline void bit_set_set_value(BitSet* const bit_set, const uint64_t value, const uint64_t index);
inline void bit_set_set(BitSet* const bit_set, const uint64_t index); 
inline void bit_set_clear(BitSet* const bit_set, const uint64_t index);
inline void bit_set_fill_all(BitSet* const bit_set, const bool value);
inline void bit_set_clear_all(BitSet* const bit_set);
inline void bit_set_set_all(BitSet* const bit_set);
inline void bit_set_fill_in_range_end(BitSet* const bit_set, const bool value, const uint64_t end);
inline void bit_set_clear_in_range_end(BitSet* const bit_set, const uint64_t end);
inline void bit_set_set_in_range_end(BitSet* const bit_set, const uint64_t end);
inline void bit_set_fill_in_range_begin_end(BitSet* const bit_set, const bool value, const uint64_t begin, const uint64_t end);
inline void bit_set_clear_in_range_begin_end(BitSet* const bit_set, const uint64_t begin, const uint64_t end);
inline void bit_set_set_in_range_begin_end(BitSet* const bit_set, const uint64_t begin, const uint64_t end);
inline void bit_set_fill_in_range_begin_end_step(BitSet* const bit_set, const bool value, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bit_set_clear_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bit_set_set_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bit_set_set_chunk(BitSet* const bit_set, const uint8_t chunk, const uint64_t index);
inline void bit_set_fill_chunk(BitSet* const bit_set, const uint8_t value);
inline void bit_set_fill_chunk_in_range_end(BitSet* const bit_set, const uint8_t chunk, const uint64_t end);
inline void bit_set_fill_chunk_in_range_begin_end(BitSet* const bit_set, const uint8_t chunk, const uint64_t begin, const uint64_t end);
inline void bit_set_fill_chunk_in_range_begin_end_step(BitSet* const bit_set, const uint8_t chunk, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bit_set_flip(BitSet* const bit_set, const uint64_t index);
inline void bit_set_flip_all(BitSet* const bit_set);
inline void bit_set_flip_in_range_end(BitSet* const bit_set, const uint64_t end);
inline void bit_set_flip_in_range_begin_end(BitSet* const bit_set, const uint64_t begin, const uint64_t end);
inline void bit_set_flip_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bit_set_flip_chunk(BitSet* const bit_set, const uint64_t index);
inline void bit_set_flip_all_chunks(BitSet* const bit_set);
inline void bit_set_flip_chunk_in_range_end(BitSet* const bit_set, const uint64_t end);
inline void bit_set_flip_chunk_in_range_begin_end(BitSet* const bit_set, const uint64_t begin, const uint64_t end);
inline void bit_set_flip_chunk_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step);
inline uint8_t bit_set_get_chunk(const BitSet* const bit_set, const uint64_t index);
inline bool bit_set_all(const BitSet* const bit_set);
inline bool bit_set_any(const BitSet* const bit_set);
inline bool bit_set_none(const BitSet* const bit_set);
inline bool bit_set_all_cleared(const BitSet* const bit_set);
inline uint64_t bit_set_count(const BitSet* const bit_set);
inline bool bit_set_empty(const BitSet* const bit_set);
inline void bit_set_push_back(BitSet* const bit_set, const bool value);
inline void bit_set_pop_back(BitSet* const bit_set);
inline void bit_set_push_back_chunk(BitSet* const bit_set, const uint8_t chunk);
inline void bit_set_pop_back_chunk(BitSet* const bit_set);
inline void bit_set_resize(BitSet* const bit_set, const uint64_t new_size);
inline const uint64_t bit_set_calculate_storage_size(const uint64_t size);

/**
 * Size initialization
 * @param bit_set The bit_set to initialize
 * @param size The size of the bit_set to be initialized
 * @struct BitSet
 */
inline void bit_set_init(BitSet* const bit_set, const uint64_t size) 
{
    bit_set->size = size;
    bit_set->storage_size = size / 8u + (size % 8u ? 1 : 0);
    bit_set->data = (uint8_t*)malloc(bit_set->storage_size * sizeof(uint8_t));
    bit_set_fill_all(bit_set, 0);
}

/**
 * Size and value initialization
 * @param bit_set The bit_set to initialize
 * @param size The size of the bit_set to be initialized
 * @param value The value to fill the bit_set with
 * @struct BitSet
 */
inline void bit_set_init_with_value(BitSet* const bit_set, const uint64_t size, const uint64_t value) 
{
    bit_set->size = size;
    bit_set->storage_size = size / 8u + (size % 8u ? 1 : 0);
    bit_set->data = (uint8_t*)malloc(bit_set->storage_size * sizeof(uint8_t));
    bit_set_fill_all(bit_set, value);
}

/**
 * Destroys the bit_set (frees the memory)
 * @param bit_set The bit_set to destroy
 * @struct BitSet
 */
inline void bit_set_destroy(const BitSet* const bit_set) 
{
    free(bit_set->data);
}

/**
 * Copies the data from one bit_set to another
 * @param destination The bit_set to copy to
 * @param source The bit_set to copy from
 * @struct BitSet
 */
inline void bit_set_copy_from(BitSet* const destination, const BitSet* const source) 
{
    for (uint64_t i = 0; i < destination->size; ++i)
        *(destination->data + i) = *(source->data + i);
}

/**
 * Moves the data from one bit_set to another
 * @param destination The bit_set to move to
 * @param source The bit_set to move from
 * @struct BitSet
 */
inline void bit_set_move_from(BitSet* const destination, BitSet* source) 
{
    destination->size = source->size;
    source->size = 0;
    destination->data = source->data;
    source->data = NULL;
}

/**
 * Sets the value of a bit at a specified index
 * @param bit_set The bit_set to modify
 * @param value The value to set the bit to
 * @param index The index of the bit to modify (bit index)
 * @struct BitSet
 */
inline void bit_set_set_value(const BitSet* const bit_set, const uint64_t value, const uint64_t index) 
{
    if (value)
        *(bit_set->data + index / sizeof(uint8_t)) |= 1 << index % sizeof(uint8_t);
    else
        *(bit_set->data + index / sizeof(uint8_t)) &= ~(1 << index % sizeof(uint8_t));
}

/**
 * Retrieves the value of a bit at a specified index
 * @param bit_set The bit_set to read from
 * @param index The index of the bit to read (bit index)
 * @return The value of the bit at the specified index
 * @struct BitSet
 */
inline bool bit_set_get(const BitSet* const bit_set, const uint64_t index) {
    return (*(bit_set->data + index / 8u) & 1 << index % 8u) >> index % 8u;
}

/**
 * Sets the value of a bit at a specified index to 1 (true)
 * @param bit_set The bit_set to modify
 * @param index The index of the bit to set (bit index)
 * @struct BitSet
 */
inline void bit_set_set(BitSet* const bit_set, const uint64_t index) {
    *(bit_set->data + index / sizeof(uint8_t)) |= 1 << index % sizeof(uint8_t);
}

/**
 * Sets the value of a bit at a specified index to 0 (false)
 * @param bit_set The bit_set to modify
 * @param index The index of the bit to clear (bit index)
 * @struct BitSet
 */
inline void bit_set_clear(BitSet* const bit_set, const uint64_t index) {
    *(bit_set->data + index / sizeof(uint8_t)) &= ~(1 << index % sizeof(uint8_t));
}

/**
 * Fills the bit_set with a specified value
 * @param bit_set The bit_set to modify
 * @param value The value to fill the bit_set with
 * @struct BitSet
 */
inline void bit_set_fill_all(BitSet* const bit_set, const bool value) {
    const uint8_t chunk = value ? 255u : 0u;
    for (uint64_t i = 0; i < bit_set->storage_size; ++i)
        *(bit_set->data + i) = chunk;
}

/**
 * Clears all the bits (sets all bits to 0)
 * @param bit_set The bit_set to modify
 * @struct BitSet
 */
inline void bit_set_clear_all(BitSet* const bit_set) {
    memset(bit_set->data, 0, bit_set->storage_size);
}

/**
 * Sets all the bits (sets all bits to 1)
 * @param bit_set The bit_set to modify
 * @struct BitSet
 */
inline void bit_set_set_all(BitSet* const bit_set) {
    memset(bit_set->data, 255, bit_set->storage_size);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bit_set The bit_set to modify
 * @param value The value to fill the bits with (bit value)
 * @param end End of the range to fill (bit index)
 * @struct BitSet
 */
inline void bit_set_fill_in_range_end(BitSet* const bit_set, const bool value, const uint64_t end) {
    for (uint64_t i = 0; i < end; ++i)
        *(bit_set->data + i) = value ? 255u : 0u;
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bit_set The bit_set to modify
 * @param end End of the range to fill (bit index)
 * @struct BitSet
 */
inline void bit_set_clear_in_range(BitSet* const bit_set, const uint64_t end)
{
    memset(bit_set->data, 0, end / 8u);
    if (end % 8u)
    {
        for (uint16_t i = 0; i < end % 8u; ++i)
            *(bit_set->data + end / 8u) &= ~(1u << i);
    }
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bit_set The bit_set to modify
 * @param end End of the range to fill (bit index)
 * @struct BitSet
 */
inline void bit_set_set_in_range(BitSet* const bit_set, const uint64_t end)
{
    memset(bit_set->data, 255u, end / 8u);
    if (end % 8u)
    {
        for (uint16_t i = 0; i < end % 8u; ++i)
            *(bit_set->data + end / 8u) |= 1u << i;
    }
}

/**
 * Fills all the bits in the specified range with the specified value
 * @param bit_set The bit_set to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @struct BitSet
 */
inline void bit_set_fill_in_range_begin_end(BitSet* const bit_set, const bool value, const uint64_t begin, const uint64_t end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_chunk and fill the first byte with it
    if (begin % 8)
    {
        const uint16_t end_bit = (begin / 8 == end / 8) ? end % 8 : 8;
        if (value)
        {
            for (uint16_t i = begin % 8; i < end_bit; ++i)
                *(bit_set->data + begin / 8) |= 1 << i;
        }
        else
        {
            for (uint16_t i = begin % 8; i < end_bit; ++i)
                *(bit_set->data + begin / 8) &= ~(1 << i);
        }
    }
    else
        to_add = 0;
    if (end % 8 && begin / 8 != end / 8)
    {
        if (value)
        {
            for (uint16_t i = 0; i < end % 8; ++i)
                *(bit_set->data + end / 8) |= 1u << i;
        }
        else
        {
            for (uint16_t i = 0; i < end % 8; ++i)
                *(bit_set->data + end / 8) &= ~(1u << i);
        }
    }
    else
        to_sub = 0;

    memset(bit_set->data + begin / 8 + to_add, value ? 255u : 0u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @struct BitSet
 */
inline void bit_set_clear_in_range_begin_end(BitSet* const bit_set, const uint64_t& begin, const uint64_t& end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_chunk and fill the first byte with it
    if (begin % 8u)
    {
        const uint16_t end_bit = begin / 8u == end / 8u ? end % 8u : 8u;
        for (uint16_t i = begin % 8u; i < end_bit; ++i)
            *(bit_set->data + begin / 8u) &= ~(1u << i);
    }
    else
        to_add = 0;

    // set the end chunk if the end is not aligned with the chunk size
    if (end % 8 && begin / 8 != end / 8)
    {
        for (uint16_t i = 0; i < end % 8; ++i)
            *(bit_set->data + end / 8) &= ~(1u << i);
    }
    else
        to_sub = 0;

    memset(bit_set->data + begin / 8u + to_add, 0u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @struct BitSet
 */
inline void bit_set_set_in_range_begin_end(BitSet* const bit_set, const uint64_t& begin, const uint64_t& end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_chunk and fill the first byte with it
    if (begin % 8u)
    {
        const uint16_t end_bit = begin / 8u == end / 8u ? end % 8u : 8u;
        for (uint16_t i = begin % 8u; i < end_bit; ++i)
            *(bit_set->data + begin / 8u) |= 1u << i;
    }
    else
        to_add = 0;

    // set the end chunk if the end is not aligned with the chunk size
    if (end % 8 && begin / 8 != end / 8)
    {
        for (uint16_t i = 0; i < end % 8; ++i)
            *(bit_set->data + end / 8) |= 1u << i;
    }
    else
        to_sub = 0;

    memset(bit_set->data + begin / 8u + to_add, 255u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with the specified value
 * @param bit_set The bit_set to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 * @struct BitSet
 */
inline void bit_set_fill_in_range_begin_end_step(BitSet* const bit_set, const bool value, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
    {
        if (value)
            *(bit_set->data + i / sizeof(uint8_t)) |= 1 << i % sizeof(uint8_t);
        else
            *(bit_set->data + i / sizeof(uint8_t)) &= ~(1 << i % sizeof(uint8_t));
    }
}

/**
 * Fills all the bits in the specified range with 0 (false
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 * @struct BitSet
 */
inline void bit_set_clear_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bit_set->data + i / sizeof(uint8_t)) &= ~(1 << i % sizeof(uint8_t));
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bit_set The bit_set to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 * @struct BitSet
 */
inline void bit_set_set_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bit_set->data + i / sizeof(uint8_t)) |= 1 << i % sizeof(uint8_t);
}

/**
 * Sets the chunk at the specified index to the specified value
 * @param bit_set The bit_set to modify
 * @param chunk The chunk to set (chunk value)
 * @param index The index of the chunk to set (chunk index)
 * @struct BitSet
 */
inline void bit_set_set_chunk(BitSet* const bit_set, const uint8_t chunk, const uint64_t index)
{
    *(bit_set->data + index) = chunk;
}

/**
 * Fills all the chunks in the specified range with the specified value
 * @param bit_set The bit_set to modify
 * @param value The value to fill the bits with (chunk value)
 * @struct BitSet
 */
inline void bit_set_fill_all_chunks(BitSet* const bit_set, const uint8_t value)
{
    for (uint64_t i = 0; i < bit_set->storage_size; ++i)
        *(bit_set->data + i) = value;
}

/**
 * Fills all the bits in the specified range with the specified chunk
 * @param bit_set The bit_set to modify
 * @param chunk Chunk to fill the bits with (chunk value)
 * @param end End of the range to fill (chunk index)
 * @struct BitSet
 */
inline void bit_set_fill_chunk_in_range_end(BitSet* const bit_set, const uint8_t chunk, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bit_set->data + i) = chunk;
}

/**
 * Fills all the bits in the specified range with the specified chunk
 * @param bit_set The bit_set to modify
 * @param chunk Chunk to fill the bits with
 * @param begin begin of the range to fill (chunk index)
 * @param end End of the range to fill (chunk index)
 * @struct BitSet
 */
inline void bit_set_fill_chunk_in_range_begin_end(BitSet* const bit_set, const uint8_t chunk, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bit_set->data + i) = chunk;
}

/**
 * Fills all the bits in the specified range with the specified chunk
 * @param bit_set The bit_set to modify
 * @param chunk Chunk to fill the bits with (chunk value)
 * @param begin begin of the range to fill (chunk index)
 * @param end End of the range to fill (chunk index)
 * @param step Step size between the bits to fill (chunk step)
 * @struct BitSet
 */
inline void bit_set_fill_chunk_in_range_begin_end_step(BitSet* const bit_set, const uint8_t chunk, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bit_set->data + i) = chunk;
}

/**
 * Flips the bit at the specified index
 * @param bit_set The bit_set to modify
 * @param index Index of the bit to flip (bit index)
 * @struct BitSet
 */
inline void bit_set_flip_bit(BitSet* const bit_set, const uint64_t index)
{
    *(bit_set->data + index / sizeof(uint8_t)) ^= 1u << index % sizeof(uint8_t);
}

/**
 * Flips all the bits
 * @param bit_set The bit_set to modify
 * @struct BitSet
 */
inline void bit_set_flip_all(BitSet* const bit_set)
{
    for (uint64_t i = 0; i < bit_set->storage_size; ++i)
        *(bit_set->data + i) = ~*(bit_set->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bit_set The bit_set to modify
 * @param end End of the range to flip (bit index)
 * @struct BitSet
 */
inline void bit_set_flip_in_range_end(BitSet* const bit_set, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bit_set->data + i) = ~*(bit_set->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to flip (bit index)
 * @param end End of the range to flip (bit index)
 * @struct BitSet
 */
inline void bit_set_flip_in_range_begin_end(BitSet* const bit_set, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bit_set->data + i) = ~*(bit_set->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to flip (bit index)
 * @param end End of the range to flip (bit index)
 * @param step Step size between the bits to flip (bit step)
 * @struct BitSet
 */
inline void bit_set_flip_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bit_set->data + i / sizeof(uint8_t)) ^= 1u << i % sizeof(uint8_t);
}

/**
 * Flips the chunk at the specified index
 * @param bit_set The bit_set to modify
 * @param index Index of the chunk to flip (chunk index)
 * @struct BitSet
 */
inline void bit_set_flip_chunk(BitSet* const bit_set, const uint64_t index)
{
    *(bit_set->data + index) = ~*(bit_set->data + index);
}

/**
 * Flips all the chunks (exactly the same as bit_set_flip_all, just alias)
 * @param bit_set The bit_set to modify
 * @struct BitSet
 */
inline void bit_set_flip_chunk_all(BitSet* const bit_set)
{
    bit_set_flip_all(bit_set);
}

/**
 * Flips all the chunks in the specified range
 * @param bit_set The bit_set to modify
 * @param end End of the range to flip (chunk index)
 * @struct BitSet
 */
inline void bit_set_flip_chunk_in_range_end(BitSet* const bit_set, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bit_set->data + i) = ~*(bit_set->data + i);
}

/**
 * Flips all the chunks in the specified range
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to flip (chunk index)
 * @param end End of the range to flip (chunk index)
 * @struct BitSet
 */
inline void bit_set_flip_chunk_in_range_begin_end(BitSet* const bit_set, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bit_set->data + i) = ~*(bit_set->data + i);
}

/**
 * Flips all the chunks in the specified range
 * @param bit_set The bit_set to modify
 * @param begin Begin of the range to flip (chunk index)
 * @param end End of the range to flip (chunk index)
 * @param step Step size between the chunks to flip (chunk step)
 * @struct BitSet
 */
inline void bit_set_flip_chunk_in_range_begin_end_step(BitSet* const bit_set, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bit_set->data + i) = ~*(bit_set->data + i);
}

/**
 * Retrieves the chunk at the specified index
 * @struct BitSet
 * @param bit_set The bit_set to read from
 * @param index Index of the chunk to read (chunk index)
 * @return The chunk at the specified index
 */
inline uint8_t bit_set_get_chunk(const BitSet* const bit_set, const uint64_t index)
{
    return *(bit_set->data + index);
}

/**
 * Checks if all the bits are set
 * @struct BitSet
 * @param bit_set The bit_set to check
 * @return True if all the bits are set, false otherwise
 */
inline bool bit_set_all(const BitSet* const bit_set)
{
    for (uint64_t i = 0; i < bit_set->storage_size - (bit_set->size % 8 ? 1 : 0); ++i)
    {
        if (*(bit_set->data + i) != 255u)
            return false;
    }
    if (bit_set->size % 8)
    {
        for (uint16_t i = 0; i < bit_set->size % 8; ++i)
        {
            if (*(bit_set->data + bit_set->storage_size - 1) & 1u << i)
                return false;
        }
    }
    return true;
}

/**
 * Checks if any of the bits are set
 * @struct BitSet
 * @param bit_set The bit_set to check
 * @return True if any of the bits are set, false otherwise
 */
inline bool bit_set_any(const BitSet* const bit_set)
{
    for (uint64_t i = 0; i < bit_set->storage_size - (bit_set->size % 8 ? 1 : 0); ++i)
    {
        if (*(bit_set->data + i))
            return true;
    }
    if (bit_set->size % 8)
    {
        for (uint16_t i = 0; i < bit_set->size % 8; ++i)
        {
            if (*(bit_set->data + bit_set->storage_size - 1) & 1u << i)
                return true;
        }
    }
    return false;
}

/**
 * Checks if none of the bits are set
 * @struct BitSet
 * @param bit_set The bit_set to check
 * @return True if none of the bits are set, false otherwise
 */
inline bool bit_set_none(const BitSet* const bit_set)
{
    return bit_set_all_cleared(bit_set);
}

/**
 * Checks if all the bits are clear
 * @struct BitSet
 * @param bit_set The bit_set to check
 * @return True if all the bits are clear, false otherwise
 */
inline bool bit_set_all_cleared(const BitSet* const bit_set)
{
    for (uint64_t i = 0; i < bit_set->storage_size - (bit_set->size % 8u ? 1 : 0); ++i)
    {
        if (*(bit_set->data + i))
            return false;
    }
    if (bit_set->size % 8)
    {
        for (uint16_t i = 0; i < bit_set->size % 8; ++i)
        {
            if (*(bit_set->data + bit_set->storage_size - 1) & 1u << i)
                return false;
        }
    }
    return true;
}

/**
 * @struct BitSet
 * @return the number of bits set in the bit_set
 */
inline uint64_t bit_set_count(const BitSet* const bit_set)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < bit_set->storage_size; ++i)
    {
        uint8_t chunk = *(bit_set->data + i);
        while (chunk)
        {
            count += chunk & 1u;
            chunk >>= 1;
        }
    }
    return count;
}

/**
 * Check if bit_set is empty
 * @param bit_set The bit_set to check
 * @return True if the bit_set is empty, false otherwise
 * @struct BitSet
*/
inline bool bit_set_empty(const BitSet* const bit_set)
{
    return !bit_set->size;
}

/**
 * Pushes back a bit to the bit_set
 * @param bit_set The bit_set to modify
 * @param value Value of the bit to append (bit value)
 * @struct BitSet
 */
void bit_set_push_back(BitSet* const bit_set, const bool value)
{
	if (bit_set->size % 8)
	{
		if (value)
			*(bit_set->data + bit_set->size / 8) |= 1u << bit_set->size % 8;
		else
			*(bit_set->data + bit_set->size / 8) &= ~(1u << bit_set->size % 8);
	}
	else
	{
		uint8_t* new_data = (uint8_t*)malloc((bit_set->storage_size + 1) * sizeof(uint8_t));
		if (bit_set->data)
		{
			memcpy(new_data, bit_set->data, bit_set->storage_size);
			free(bit_set->data);
		}
		bit_set->data = new_data;
		*(bit_set->data + bit_set->storage_size++) = value ? 1u : 0u;
	}
	++bit_set->size;
}

/**
 * Removes the last bit from the bit_set
 * @param bit_set The bit_set to modify
 * @struct BitSet
 */
void bit_set_pop_back(BitSet* const bit_set)
{
    if (bit_set->data)
    {
        if (!(bit_set->size % 8))
        {
            uint8_t* new_data = (uint8_t*)malloc((bit_set->storage_size - 1) * sizeof(uint8_t));
            memcpy(new_data, bit_set->data, bit_set->storage_size - 1);
            free(bit_set->data);
            bit_set->data = new_data;
        }
        --bit_set->size;
    }
    // else throw exception in safe version
}

/**
 * Pushes back a chunk to the bit_set, adjusting the size to the nearest multiple of sizeof(T) upwards. [e.g. 65 bits -> (+8 {chunk} +7 {expanded area} = +15) -> 80 bits]
 * The bits in the expanded area may be initialized by previous calls, but their values are not explicitly defined by this function.
 * @struct BitSet
 * @param bit_set The bit_set to modify
 * @param chunk The chunk to push back (chunk value)
 */
void bit_set_push_back_chunk(BitSet* const bit_set, const uint8_t chunk)
{
    uint8_t* new_data = (uint8_t*)malloc(bit_set->storage_size + (bit_set->size % 8 ? 2 : 1));
    if (bit_set->data)
    {
        memcpy(new_data, bit_set->data, bit_set->storage_size);
        free(bit_set->data);
    }
    bit_set->data = new_data;
    *(bit_set->data + bit_set->storage_size++) = chunk;
    bit_set->size += 8 + (bit_set->size % 8 ? 8 - bit_set->size % 8 : 0);
}

/**
 * Removes the last chunk from the bit_set, adjusting the size to the nearest lower multiple of sizeof(T). [e.g. 65 bits -> 64 bits -> 56 bits]
 * @struct BitSet
 * @param bit_set The bit_set to modify
 */
void bit_set_pop_back_chunk(BitSet* const bit_set)
{
	if (bit_set->data)
	{
		if (bit_set->size % 8)
		{
			uint8_t* new_data = (uint8_t*)malloc((bit_set->storage_size - 1) * sizeof(uint8_t));
			memcpy(new_data, bit_set->data, bit_set->storage_size - 1);
			free(bit_set->data);
			bit_set->data = new_data;
		}
		bit_set->size -= 8;
	}
	// else throw exception in safe version
}

/**
 * Resizes the bit_set to the specified size
 * @struct BitSet
 * @param bit_set The bit_set to resize
 * @param new_size The new size of the bit_set (bit size)
 */
void bit_set_resize(BitSet* const bit_set, const uint64_t new_size)
{
	if (new_size == bit_set->size)
		return;

	const uint64_t new_storage_size = bit_set_calculate_storage_size(new_size);
	uint8_t* new_data = (uint8_t*)malloc(new_storage_size * sizeof(uint8_t));
	if (bit_set->data)
	{
		memcpy(new_data, bit_set->data, new_storage_size);
		free(bit_set->data);
	}
	bit_set->data = new_data;
	bit_set->storage_size = new_storage_size;
	bit_set->size = new_size;
}

/**
 * Calculates the number of bytes required to store the bit_set
 * @struct BitSet
 * @param size The size of the bit_set
 * @return The number of bytes required to store the bit_set
 */
inline const uint64_t bit_set_calculate_storage_size(const uint64_t size)
{
    return size / 8u + (size % 8u ? 1 : 0);
}