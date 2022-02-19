<?php

/** @generate-class-entries */

namespace Teds;

/**
 * This exposes a similar API to Vector/LowMemoryVector, but for booleans instead of integers/bits.
 *
 * Benefits:
 *
 * - This is guaranteed to use less memory when serialized compared to vectors/arrays.
 * - Prevents using other value types
 */
final class BitSet implements \IteratorAggregate, Sequence, \JsonSerializable
{
    /**
     * Construct a BitSet from an iterable of booleans.
     *
     * The keys will be ignored, and values will be reindexed without gaps starting from 0
     * @psalm-param iterator<bool> $iterator
     */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator that will return the indexes and bits of iterable until index >= count()
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of bits in this BitSet
     */
    public function count(): int {}
    /**
     * Returns whether this vector is empty (has a count of 0)
     */
    public function isEmpty(): bool {}
    /**
     * Returns the total capacity of this BitSet.
     */
    public function capacity(): int {}

    /** Returns an array containing the representation type used and a little-endian binary representation of the data. */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}

    /** Create this from an array */
    public static function __set_state(array $array): BitSet {}

    public function push(mixed ...$values): void {}
    /**
     * TODO: optimize unshift(), this is currently inefficient.
     */
    public function unshift(mixed ...$values): void {}
    public function pushBits(bool ...$values): void {}

    /**
     * Returns the first bit of this bitset.
     * @throws \UnderflowException if there are no elements
     */
    public function first(): bool {}
    /**
     * Returns the last bit of this bitset.
     * @throws \UnderflowException if there are no elements
     */
    public function last(): bool {}

    /**
     * @throws \UnderflowException if there are no more elements
     */
    public function pop(): bool {}
    public function shift(): bool {}

    /** @psalm-return list<int> */
    public function toArray(): array {}
    /** @implementation-alias Teds\BitSet::toArray */
    public function values(): array {}
    public function clear(): void {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function get(int $offset): bool {}
    /**
     * Must be mixed $value to implement Sequence
     */
    public function set(int $offset, mixed $value): void {}
    public function setBit(int $offset, bool $value): void {}

    /**
     * Returns the value at (int)$offset.
     * @psalm-param int $offset
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetGet(mixed $offset): mixed {}

    /**
     * Returns true if `0 <= (int)$offset && (int)$offset < $this->count().
     * @psalm-param int $offset
     */
    public function offsetExists(mixed $offset): bool {}
    /**
     * Returns true if `is_int($offset) && 0 <= $offset && $offset < $this->count().
     */
    public function containsKey(mixed $offset): bool {}

    /**
     * Sets the value at offset (int)$offset to $value
     * @psalm-param int $offset
     * @psalm-param bool $value
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetSet(mixed $offset, mixed $value): void {}

    /**
     * @throws \RuntimeException unconditionally because unset and null are different things.
     * Also, this can only contain booleans.
     */
    public function offsetUnset(mixed $offset): void {}

    /**
     * Returns the offset of a value that is === $value, or returns null.
     */
    public function indexOf(bool $value): ?int {}
    /**
     * Returns true if there exists an boolean === $value in this BitSet.
     */
    public function contains(mixed $value): bool {}

    /**
     * @implementation-alias Teds\BitSet::toArray
     */
    public function jsonSerialize(): array {}

    public function setSize(int $size, bool $default = false): void {}
}
