#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef BITSET_SIZE
#define BITSET_SIZE 1 // to avoid compilation errors
#endif

/**
 * A dynamic bitset structure (for C API bitset)
 */
typedef struct 
{
	/**
	 * Underlying array of bytes containing the bits
	 */
	uint8_t* data;
    /**
     * Size of bitset in bits
     */
    uint64_t size;
    /**
     * Size of bitset in bytes
     */
	uint64_t storage_size;
} DynamicBitSet;

/**
 * A bitset structure (for C API bitset)
 * To use, define BITSET_SIZE to the size of the bitset you want to use
 */
typedef struct
{
    /**
     * Underlying array of bytes containing the bits
     */
    uint8_t data[BITSET_SIZE];
    /**
     * Size of bitset in bits
     */
    uint64_t size;
    /**
     * Size of bitset in bytes
    */
    uint64_t storage_size;
} BitSet;

/**
 * Casts the bitset to the universal bitset type (used with dynamic bitset)
 * @param bitset The bitset to cast
 */
#define UNIVERSAL_BITSET(bitset) (BitSet*)(bitset)

inline void bitset_dynamic_init(DynamicBitSet* const bitset, const uint64_t size);
inline void bitset_init(BitSet* const bitset);
inline void bitset_dynamic_init_block(DynamicBitSet* const bitset, const uint64_t size, const uint8_t block);
inline void bitset_init_block(BitSet* const bitset, const uint8_t block);
inline void bitset_dynamic_destroy(const DynamicBitSet* const bitset);
inline void bitset_copy_from(BitSet* const destination, const BitSet* const source);
inline void bitset_dynamic_move_from(DynamicBitSet* const destination, DynamicBitSet* const source);
inline bool bitset_get(const BitSet* const bitset, const uint64_t index);
inline void bitset_set_value(BitSet* const bitset, const uint64_t value, const uint64_t index);
inline void bitset_set(BitSet* const bitset, const uint64_t index); 
inline void bitset_clear(BitSet* const bitset, const uint64_t index);
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
inline void bitset_set_block(BitSet* const bitset, const uint8_t block, const uint64_t index);
inline void bitset_fill_block(BitSet* const bitset, const uint8_t value);
inline void bitset_fill_block_in_range_end(BitSet* const bitset, const uint8_t block, const uint64_t end);
inline void bitset_fill_block_in_range_begin_end(BitSet* const bitset, const uint8_t block, const uint64_t begin, const uint64_t end);
inline void bitset_fill_block_in_range_begin_end_step(BitSet* const bitset, const uint8_t block, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_flip(BitSet* const bitset, const uint64_t index);
inline void bitset_flip_all(BitSet* const bitset);
inline void bitset_flip_in_range_end(BitSet* const bitset, const uint64_t end);
inline void bitset_flip_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end);
inline void bitset_flip_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step);
inline void bitset_flip_block(BitSet* const bitset, const uint64_t index);
inline void bitset_flip_all_blocks(BitSet* const bitset);
inline void bitset_flip_block_in_range_end(BitSet* const bitset, const uint64_t end);
inline void bitset_flip_block_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end);
inline void bitset_flip_block_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step);
inline uint8_t bitset_get_block(const BitSet* const bitset, const uint64_t index);
inline bool bitset_all(const BitSet* const bitset);
inline bool bitset_any(const BitSet* const bitset);
inline bool bitset_none(const BitSet* const bitset);
inline bool bitset_all_cleared(const BitSet* const bitset);
inline uint64_t bitset_count(const BitSet* const bitset);
inline bool bitset_empty(const BitSet* const bitset);
inline void bitset_dynamic_push_back(DynamicBitSet* const bitset, const bool value);
inline void bitset_dynamic_pop_back(DynamicBitSet* const bitset);
inline void bitset_dynamic_push_back_block(DynamicBitSet* const bitset, const uint8_t block);
inline void bitset_dynamic_pop_back_block(DynamicBitSet* const bitset);
inline void bitset_dynamic_resize(DynamicBitSet* const bitset, const uint64_t new_size);
inline const uint64_t bitset_calculate_storage_size(const uint64_t size);
inline const uint8_t bitset_create_filled_block(const bool value);

/**
 * Size initialization
 * @param bitset Pointer to bitset to initialize
 * @param size The size of the bitset to be initialized
 * @memberof DynamicBitSet
 */
inline void bitset_dynamic_init(DynamicBitSet* const bitset, const uint64_t size) 
{
    bitset->size = size;
    bitset->storage_size = size / 8u + (size % 8u ? 1 : 0);
    bitset->data = (uint8_t*)calloc(bitset->storage_size * sizeof(uint8_t), sizeof(uint8_t));
}

/**
 * Size initialization
 * @param bitset Pointer to bitset to initialize
 * @param size The size of the bitset to be initialized
 * @memberof BitSet
 */
inline void bitset_init(BitSet* const bitset)
{
    bitset->size = BITSET_SIZE;
    bitset->storage_size = BITSET_SIZE / 8u + (BITSET_SIZE % 8u ? 1 : 0);
    memset(bitset->data, 0, BITSET_SIZE * sizeof(uint8_t));
}

/**
 * Size and value initialization
 * @param bitset Pointer to bitset to initialize
 * @param size The size of the bitset to be initialized
 * @param block The block to fill the bitset with (block value)
 * @memberof DynamicBitSet
 */
inline void bitset_dynamic_init_block(DynamicBitSet* const bitset, const uint64_t size, const uint8_t block) 
{
    bitset->size = size;
    bitset->storage_size = size / 8u + (size % 8u ? 1 : 0);
    bitset->data = (uint8_t*)malloc(bitset->storage_size * sizeof(uint8_t));
    memset(bitset->data, block, bitset->storage_size * sizeof(uint8_t));
}

/**
 * Size and value initialization
 * @param bitset Pointer to bitset to initialize
 * @param size The size of the bitset to be initialized
 * @param block The block to fill the bitset with (block value)
 * @memberof BitSet
 */
inline void bitset_init_block(BitSet* const bitset, const uint8_t block)
{
    bitset->size = BITSET_SIZE;
    bitset->storage_size = BITSET_SIZE / 8u + (BITSET_SIZE % 8u ? 1 : 0);
    memset(bitset->data, block, BITSET_SIZE * sizeof(uint8_t));
}

/**
 * Destroys the bitset (frees the memory)
 * @param bitset Pointer to bitset to destroy
 * @memberof BitSet
 */
inline void bitset_dynamic_destroy(DynamicBitSet* bitset) 
{
    free(bitset->data);
}

/**
 * Copies the data from one bitset to another
 * @param destination Pointer to bitset to copy to
 * @param source Pointer to bitset to copy from
 * @memberof BitSet
 */
inline void bitset_copy(BitSet* const destination, const BitSet* const source) 
{
    for (uint64_t i = 0; i < destination->size; ++i)
        *(destination->data + i) = *(source->data + i);
}

/**
 * Moves the data from one bitset to another
 * @param destination Pointer to bitset to move to
 * @param source Pointer to bitset to move from
 * @memberof BitSet
 */
inline void bitset_dynamic_move(DynamicBitSet* const destination, DynamicBitSet* const source) 
{
    destination->size = source->size;
    destination->data = source->data;
    source->size = 0;
    source->data = NULL;
}

/**
 * Sets the value of a bit at a specified index
 * @param bitset Pointer to bitset to modify
 * @param value The value to set the bit to
 * @param index The index of the bit to modify (bit index)
 * @memberof BitSet
 */
inline void bitset_set_value(BitSet* const bitset, const uint64_t value, const uint64_t index) 
{
    if (value)
        *(bitset->data + index / 8) |= (uint8_t)1u << index % sizeof(uint8_t);
    else
        *(bitset->data + index / 8) &= ~((uint8_t)1u << index % sizeof(uint8_t));
}

/**
 * Retrieves the value of a bit at a specified index
 * @param bitset Pointer to bitset to read from
 * @param index The index of the bit to read (bit index)
 * @return The value of the bit at the specified index
 * @memberof BitSet
 */
inline bool bitset_get(const BitSet* const bitset, const uint64_t index) {
    return (*(bitset->data + index / 8u) & (uint8_t)1u << index % 8u);
}

/**
 * Sets the value of a bit at a specified index to 1 (true)
 * @param bitset Pointer to bitset to modify
 * @param index The index of the bit to set (bit index)
 * @memberof BitSet
 */
inline void bitset_set(BitSet* const bitset, const uint64_t index) {
    *(bitset->data + index / 8) |= (uint8_t)(uint8_t)1u << index % 8;
}

/**
 * Sets the value of a bit at a specified index to 0 (false)
 * @param bitset Pointer to bitset to modify
 * @param index The index of the bit to clear (bit index)
 * @memberof BitSet
 */
inline void bitset_clear(BitSet* const bitset, const uint64_t index) {
    *(bitset->data + index / 8) &= ~((uint8_t)1u << index % 8);
}

/**
 * Fills the bitset with a specified value
 * @param bitset Pointer to bitset to modify
 * @param value The value to fill the bitset with
 * @memberof BitSet
 */
inline void bitset_fill_all(BitSet* const bitset, const bool value) {
    memset(bitset->data, value ? 255u : 0u, bitset->size);
}

/**
 * Clears all the bits (sets all bits to 0)
 * @param bitset Pointer to bitset to modify
 * @memberof BitSet
 */
inline void bitset_clear_all(BitSet* const bitset) {
    memset(bitset->data, 0, bitset->storage_size);
}

/**
 * Sets all the bits (sets all bits to 1)
 * @param bitset Pointer to bitset to modify
 * @memberof BitSet
 */
inline void bitset_set_all(BitSet* const bitset) {
    memset(bitset->data, 255, bitset->storage_size);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset Pointer to bitset to modify
 * @param value The value to fill the bits with (bit value)
 * @param end End of the range to fill (bit index)
 * @memberof BitSet
 */
inline void bitset_fill_in_range_end(BitSet* const bitset, const bool value, const uint64_t end) {
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = value ? 255u : 0u;
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bitset Pointer to bitset to modify
 * @param end End of the range to fill (bit index)
 * @memberof BitSet
 */
inline void bitset_clear_in_range_end(BitSet* const bitset, const uint64_t end)
{
    memset(bitset->data, 0, end / 8u);
    if (end % 8u)
    {
        for (uint16_t i = 0; i < end % 8u; ++i)
            *(bitset->data + end / 8u) &= ~((uint8_t)1u << i);
    }
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset Pointer to bitset to modify
 * @param end End of the range to fill (bit index)
 * @memberof BitSet
 */
inline void bitset_set_in_range_end(BitSet* const bitset, const uint64_t end)
{
    memset(bitset->data, 255u, end / 8u);
    if (end % 8u)
    {
        for (uint16_t i = 0; i < end % 8u; ++i)
            *(bitset->data + end / 8u) |= (uint8_t)1u << i;
    }
}

/**
 * Fills all the bits in the specified range with the specified value
 * @param bitset Pointer to bitset to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @memberof BitSet
 */
inline void bitset_fill_in_range_begin_end(BitSet* const bitset, const bool value, const uint64_t begin, const uint64_t end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_block and fill the first byte with it
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
                *(bitset->data + end / 8) |= (uint8_t)1u << i;
        }
        else
        {
            for (uint16_t i = 0; i < end % 8; ++i)
                *(bitset->data + end / 8) &= ~((uint8_t)1u << i);
        }
    }
    else
        to_sub = 0;

    memset(bitset->data + begin / 8 + to_add, value ? 255u : 0u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with 0 (false)
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @memberof BitSet
 */
inline void bitset_clear_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_block and fill the first byte with it
    if (begin % 8u)
    {
        const uint16_t end_bit = begin / 8u == end / 8u ? end % 8u : 8u;
        for (uint16_t i = begin % 8u; i < end_bit; ++i)
            *(bitset->data + begin / 8u) &= ~((uint8_t)1u << i);
    }
    else
        to_add = 0;

    // set the end block if the end is not aligned with the block size
    if (end % 8 && begin / 8 != end / 8)
    {
        for (uint16_t i = 0; i < end % 8; ++i)
            *(bitset->data + end / 8) &= ~((uint8_t)1u << i);
    }
    else
        to_sub = 0;

    memset(bitset->data + begin / 8u + to_add, 0u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @memberof BitSet
 */
inline void bitset_set_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end)
{
    uint8_t to_add = 1, to_sub = 1;
    // create begin_block and fill the first byte with it
    if (begin % 8u)
    {
        const uint16_t end_bit = begin / 8u == end / 8u ? end % 8u : 8u;
        for (uint16_t i = begin % 8u; i < end_bit; ++i)
            *(bitset->data + begin / 8u) |= (uint8_t)1u << i;
    }
    else
        to_add = 0;

    // set the end block if the end is not aligned with the block size
    if (end % 8 && begin / 8 != end / 8)
    {
        for (uint16_t i = 0; i < end % 8; ++i)
            *(bitset->data + end / 8) |= (uint8_t)1u << i;
    }
    else
        to_sub = 0;

    memset(bitset->data + begin / 8u + to_add, 255u, (end - begin) / 8u - to_sub);
}

/**
 * Fills all the bits in the specified range with the specified value
 * @param bitset Pointer to bitset to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 * @memberof BitSet
 */
inline void bitset_fill_in_range_begin_end_step(BitSet* const bitset, const bool value, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
    {
        if (value)
            *(bitset->data + i / 8) |= (uint8_t)1u << i % 8;
        else
            *(bitset->data + i / 8) &= ~((uint8_t)1u << i % 8);
    }
}

/**
 * Fills all the bits in the specified range with 0 (false
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 * @memberof BitSet
 */
inline void bitset_clear_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i / 8) &= ~((uint8_t)1u << i % 8);
}

/**
 * Fills all the bits in the specified range with 1 (true)
 * @param bitset Pointer to bitset to modify
 * @param value Value to fill the bits with (bit value)
 * @param begin Begin of the range to fill (bit index)
 * @param end End of the range to fill (bit index)
 * @param step Step size between the bits to fill (bit step)
 * @memberof BitSet
 */
inline void bitset_set_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i / 8) |= (uint8_t)1u << i % 8;
}

/**
 * Sets the block at the specified index to the specified value
 * @param bitset Pointer to bitset to modify
 * @param block The block to set (block value)
 * @param index The index of the block to set (block index)
 * @memberof BitSet
 */
inline void bitset_set_block(BitSet* const bitset, const uint8_t block, const uint64_t index)
{
    *(bitset->data + index) = block;
}

/**
 * Fills all the blocks in the specified range with the specified value
 * @param bitset Pointer to bitset to modify
 * @param value The value to fill the bits with (block value)
 * @memberof BitSet
 */
inline void bitset_fill_all_blocks(BitSet* const bitset, const uint8_t value)
{
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
        *(bitset->data + i) = value;
}

/**
 * Fills all the bits in the specified range with the specified block
 * @param bitset Pointer to bitset to modify
 * @param block Chunk to fill the bits with (block value)
 * @param end End of the range to fill (block index)
 * @memberof BitSet
 */
inline void bitset_fill_block_in_range_end(BitSet* const bitset, const uint8_t block, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = block;
}

/**
 * Fills all the bits in the specified range with the specified block
 * @param bitset Pointer to bitset to modify
 * @param block Chunk to fill the bits with
 * @param begin begin of the range to fill (block index)
 * @param end End of the range to fill (block index)
 * @memberof BitSet
 */
inline void bitset_fill_block_in_range_begin_end(BitSet* const bitset, const uint8_t block, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bitset->data + i) = block;
}

/**
 * Fills all the bits in the specified range with the specified block
 * @param bitset Pointer to bitset to modify
 * @param block Chunk to fill the bits with (block value)
 * @param begin begin of the range to fill (block index)
 * @param end End of the range to fill (block index)
 * @param step Step size between the bits to fill (block step)
 * @memberof BitSet
 */
inline void bitset_fill_block_in_range_begin_end_step(BitSet* const bitset, const uint8_t block, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i) = block;
}

/**
 * Flips the bit at the specified index
 * @param bitset Pointer to bitset to modify
 * @param index Index of the bit to flip (bit index)
 * @memberof BitSet
 */
inline void bitset_flip_bit(BitSet* const bitset, const uint64_t index)
{
    *(bitset->data + index / sizeof(uint8_t)) ^= (uint8_t)1u << index % sizeof(uint8_t);
}

/**
 * Flips all the bits
 * @param bitset Pointer to bitset to modify
 * @memberof BitSet
 */
inline void bitset_flip_all(BitSet* const bitset)
{
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bitset Pointer to bitset to modify
 * @param end End of the range to flip (bit index)
 * @memberof BitSet
 */
inline void bitset_flip_in_range_end(BitSet* const bitset, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to flip (bit index)
 * @param end End of the range to flip (bit index)
 * @memberof BitSet
 */
inline void bitset_flip_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the bits in the specified range
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to flip (bit index)
 * @param end End of the range to flip (bit index)
 * @param step Step size between the bits to flip (bit step)
 * @memberof BitSet
 */
inline void bitset_flip_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i / sizeof(uint8_t)) ^= (uint8_t)1u << i % sizeof(uint8_t);
}

/**
 * Flips the block at the specified index
 * @param bitset Pointer to bitset to modify
 * @param index Index of the block to flip (block index)
 * @memberof BitSet
 */
inline void bitset_flip_block(BitSet* const bitset, const uint64_t index)
{
    *(bitset->data + index) = ~*(bitset->data + index);
}

/**
 * Flips all the blocks (exactly the same as bitset_flip_all, just alias)
 * @param bitset Pointer to bitset to modify
 * @memberof BitSet
 */
inline void bitset_flip_block_all(BitSet* const bitset)
{
    bitset_flip_all(bitset);
}

/**
 * Flips all the blocks in the specified range
 * @param bitset Pointer to bitset to modify
 * @param end End of the range to flip (block index)
 * @memberof BitSet
 */
inline void bitset_flip_block_in_range_end(BitSet* const bitset, const uint64_t end)
{
    for (uint64_t i = 0; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the blocks in the specified range
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to flip (block index)
 * @param end End of the range to flip (block index)
 * @memberof BitSet
 */
inline void bitset_flip_block_in_range_begin_end(BitSet* const bitset, const uint64_t begin, const uint64_t end)
{
    for (uint64_t i = begin; i < end; ++i)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Flips all the blocks in the specified range
 * @param bitset Pointer to bitset to modify
 * @param begin Begin of the range to flip (block index)
 * @param end End of the range to flip (block index)
 * @param step Step size between the blocks to flip (block step)
 * @memberof BitSet
 */
inline void bitset_flip_block_in_range_begin_end_step(BitSet* const bitset, const uint64_t begin, const uint64_t end, const uint64_t step)
{
    for (uint64_t i = begin; i < end; i += step)
        *(bitset->data + i) = ~*(bitset->data + i);
}

/**
 * Retrieves the block at the specified index
 * @memberof BitSet
 * @param bitset Pointer to bitset to read from
 * @param index Index of the block to read (block index)
 * @return The block at the specified index
 */
inline uint8_t bitset_get_block(const BitSet* const bitset, const uint64_t index)
{
    return *(bitset->data + index);
}

/**
 * Checks if all the bits are set
 * @memberof BitSet
 * @param bitset Pointer to bitset to check
 * @return True if all the bits are set, false otherwise
 */
inline bool bitset_all(const BitSet* const bitset)
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
            if (*(bitset->data + bitset->storage_size - 1) & (uint8_t)1u << i)
                return false;
        }
    }
    return true;
}

/**
 * Checks if any of the bits are set
 * @memberof BitSet
 * @param bitset Pointer to bitset to check
 * @return True if any of the bits are set, false otherwise
 */
inline bool bitset_any(const BitSet* const bitset)
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
            if (*(bitset->data + bitset->storage_size - 1) & (uint8_t)1u << i)
                return true;
        }
    }
    return false;
}

/**
 * Checks if none of the bits are set
 * @memberof BitSet
 * @param bitset Pointer to bitset to check
 * @return True if none of the bits are set, false otherwise
 */
inline bool bitset_none(const BitSet* const bitset)
{
    return bitset_all_cleared(bitset);
}

/**
 * Checks if all the bits are clear
 * @memberof BitSet
 * @param bitset Pointer to bitset to check
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
            if (*(bitset->data + bitset->storage_size - 1) & (uint8_t)1u << i)
                return false;
        }
    }
    return true;
}

/**
 * @memberof BitSet
 * @return the number of bits set in the bitset
 */
inline uint64_t bitset_count(const BitSet* const bitset)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < bitset->storage_size; ++i)
    {
        uint8_t block = *(bitset->data + i);
        while (block)
        {
            count += block & (uint8_t)1u;
            block >>= 1;
        }
    }
    return count;
}

/**
 * Check if bitset is empty
 * @param bitset Pointer to bitset to check
 * @return True if the bitset is empty, false otherwise
 * @memberof BitSet
*/
inline bool bitset_empty(const BitSet* const bitset)
{
    return !bitset->size;
}

/**
 * Pushes back a bit to the bitset
 * @param bitset Pointer to bitset to modify
 * @param value Value of the bit to append (bit value)
 * @memberof BitSet
 */
void bitset_dynamic_push_back(DynamicBitSet* const bitset, const bool value)
{
	if (bitset->size % 8)
	{
		if (value)
			*(bitset->data + bitset->size / 8) |= (uint8_t)1u << bitset->size % 8;
		else
			*(bitset->data + bitset->size / 8) &= ~((uint8_t)1u << bitset->size % 8);
	}
	else
	{
		uint8_t* new_data = (uint8_t*)malloc((bitset->storage_size + 1) * sizeof(uint8_t));
		if (bitset->data)
		{
			memcpy(new_data, bitset->data, bitset->storage_size);
			free(bitset->data);
		}
		bitset->data = new_data;
		*(bitset->data + bitset->storage_size++) = value ? 1u : 0u;
	}
	++bitset->size;
}

/**
 * Removes the last bit from the bitset
 * @param bitset Pointer to bitset to modify
 * @memberof BitSet
 */
void bitset_dynamic_pop_back(DynamicBitSet* const bitset)
{
    if (bitset->data)
    {
        if (!(bitset->size % 8))
        {
            uint8_t* new_data = (uint8_t*)malloc((bitset->storage_size - 1) * sizeof(uint8_t));
            memcpy(new_data, bitset->data, bitset->storage_size - 1);
            free(bitset->data);
            bitset->data = new_data;
        }
        --bitset->size;
    }
    // else throw exception in safe version
}

/**
 * Pushes back a block to the bitset, adjusting the size to the nearest multiple of sizeof(T) upwards. [e.g. 65 bits -> (+8 {block} +7 {expanded area} = +15) -> 80 bits]
 * The bits in the expanded area may be initialized by previous calls, but their values are not explicitly defined by this function.
 * @memberof BitSet
 * @param bitset Pointer to bitset to modify
 * @param block The block to push back (block value)
 */
void bitset_dynamic_push_back_block(DynamicBitSet* const bitset, const uint8_t block)
{
    uint8_t* new_data = (uint8_t*)malloc(bitset->storage_size + (bitset->size % 8 ? 2 : 1));
    if (bitset->data)
    {
        memcpy(new_data, bitset->data, bitset->storage_size);
        free(bitset->data);
    }
    bitset->data = new_data;
    *(bitset->data + bitset->storage_size++) = block;
    bitset->size += 8 + (bitset->size % 8 ? 8 - bitset->size % 8 : 0);
}

/**
 * Removes the last block from the bitset, adjusting the size to the nearest lower multiple of sizeof(T). [e.g. 65 bits -> 64 bits -> 56 bits]
 * @memberof BitSet
 * @param bitset Pointer to bitset to modify
 */
void bitset_dynamic_pop_back_block(DynamicBitSet* const bitset)
{
	if (bitset->data)
	{
		if (bitset->size % 8)
		{
			uint8_t* new_data = (uint8_t*)malloc((bitset->storage_size - 1) * sizeof(uint8_t));
			memcpy(new_data, bitset->data, bitset->storage_size - 1);
			free(bitset->data);
			bitset->data = new_data;
		}
		bitset->size -= 8;
	}
	// else throw exception in safe version
}

/**
 * Resizes the bitset to the specified size
 * @memberof BitSet
 * @param bitset Pointer to bitset to resize
 * @param new_size The new size of the bitset (bit size)
 */
void bitset_dynamic_resize(DynamicBitSet* const bitset, const uint64_t new_size)
{
	if (new_size == bitset->size)
		return;

	const uint64_t new_storage_size = bitset_calculate_storage_size(new_size);
	uint8_t* new_data = (uint8_t*)malloc(new_storage_size * sizeof(uint8_t));
	if (bitset->data)
	{
		memcpy(new_data, bitset->data, new_storage_size);
		free(bitset->data);
	}
	bitset->data = new_data;
	bitset->storage_size = new_storage_size;
	bitset->size = new_size;
}

/**
 * Calculates the number of bytes required to store the bitset
 * @memberof BitSet
 * @param size The size of the bitset
 * @return The number of bytes required to store the bitset
 */
inline const uint64_t bitset_calculate_storage_size(const uint64_t size)
{
    return size / 8u + (size % 8u ? 1 : 0);
}

/**
 * Creates a block of type uint8_t based on the given boolean value.
 *
 * @param value A boolean value indicating whether to create the block with the maximum value or zero.
 * @return The created block of type uint8_t. If value is true, returns the maximum value representable by type uint8_t, which is 255,
 *         otherwise returns zero.
 */
inline const uint8_t bitset_create_filled_block(const bool value)
{
    return value ? 255u : 0u;
}