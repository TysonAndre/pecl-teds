<?php

/** @generate-class-entries */

namespace Teds;

/**
 * This exposes the same API as a Vector/LowMemoryVector, but with a more memory-efficient representation.
 *
 * Benefits:
 *
 * - This is guaranteed to use less memory when serialized compared to vectors/arrays.
 * - Prevents using other value types
 *
 * In 64-bit builds, the following types are supported, with the following amounts of memory (plus constant overhead to represent the IntVector itself, and extra capacity for growing the IntVector):
 *
 * 1. signed 8-bit value (-128..127): 1 byte per value. Using larger values changes the representation to the new representation.
 * 2. signed 16-bit value: 2 bytes per value.
 * 3. signed 32-bit value: 4 bytes per value.
 * 4. signed 64-bit value: 8 bytes per value. (64-bit builds only)
 *
 * In comparison, in 64-bit builds of PHP, PHP's arrays take at least 16 bytes per value in php 8.2, and at least 32 bytes per value before php 8.1, at the time of writing.
 */
final class IntVector implements \IteratorAggregate, Sequence, \JsonSerializable
{
    /**
     * Construct a IntVector from an iterable of integers.
     *
     * The keys will be ignored, and values will be reindexed without gaps starting from 0
     * @psalm-param iterator<int> $iterator
     */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator that will return the indexes and values of iterable until index >= count()
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of values in this IntVector
     */
    public function count(): int {}
    /**
     * Returns whether this vector is empty (has a count of 0)
     */
    public function isEmpty(): bool {}
    /**
     * Returns the total capacity of this IntVector.
     */
    public function capacity(): int {}

    /** Returns an array containing the representation type used and a little-endian binary representation of the data. */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}

    /** Create this from an array */
    public static function __set_state(array $array): IntVector {}

    public function push(mixed ...$values): void {}
    public function pushInts(int ...$values): void {}
    /**
     * @throws \RuntimeException if there are no more elements
     */
    public function pop(): int {}

    /** @psalm-return list<int> */
    public function toArray(): array {}
    /** @implementation-alias Teds\IntVector::toArray */
    public function values(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function get(int $offset): int {}
    /**
     * Must be mixed $value to implement Sequence
     */
    public function set(int $offset, mixed $value): void {}
    public function setInt(int $offset, int $value): void {}

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
     * @psalm-param int $value
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetSet(mixed $offset, mixed $value): void {}

    /**
     * @throws \RuntimeException unconditionally because unset and null are different things.
     * Also, this can only contain integers.
     */
    public function offsetUnset(mixed $offset): void {}

    /**
     * Returns the offset of a value that is === $value, or returns null.
     */
    public function indexOf(int $value): ?int {}
    /**
     * Returns true if there exists an integer === $value in this IntVector.
     */
    public function contains(mixed $value): bool {}

    /**
     * @implementation-alias Teds\IntVector::toArray
     */
    public function jsonSerialize(): array {}
}
