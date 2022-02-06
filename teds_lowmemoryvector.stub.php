<?php

/** @generate-class-entries */

namespace Teds;

/**
 * A LowMemoryVector is a container of a sequence of values (with keys 0, 1, ...count($vector) - 1)
 * that can change in size.
 *
 * This is backed by a memory-efficient representation
 * (raw array of values) and provides fast access (compared to other objects in the Spl)
 * and constant amortized-time push/pop operations.
 *
 * Attempting to read or write values outside of the range of values with `*get`/`*set` methods will throw at runtime.
 *
 * **UNSERIALIZATION IS NOT IMPLEMENTED YET**
 */
final class LowMemoryVector implements \IteratorAggregate, \Countable, \JsonSerializable, \ArrayAccess
{
    /**
     * Construct a LowMemoryVector from an iterable.
     *
     * The keys will be ignored, and values will be reindexed without gaps starting from 0
     */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator that will return the indexes and values of iterable until index >= count()
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of values in this LowMemoryVector
     */
    public function count(): int {}
    /**
     * Returns whether this vector is empty (has a count of 0)
     */
    public function isEmpty(): bool {}
    /**
     * Returns the total capacity of this LowMemoryVector.
     */
    public function capacity(): int {}

    public function __serialize(): array {}
    /**
     * FIXME finalize implementation and implement remaining types before enabling unserialization
     * @throws \RuntimeException unconditionally.
     */
    public function __unserialize(array $data): void {}

    /** Create this from an array */
    public static function __set_state(array $array): LowMemoryVector {}

    public function push(mixed ...$values): void {}
    public function pop(): mixed {}

    public function toArray(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function get(int $offset): mixed {}
    public function set(int $offset, mixed $value): void {}

    /**
     * Returns the value at (int)$offset.
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetGet(mixed $offset): mixed {}

    /**
     * Returns true if `0 <= (int)$offset && (int)$offset < $this->count().
     */
    public function offsetExists(mixed $offset): bool {}

    /**
     * Sets the value at offset (int)$offset to $value
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetSet(mixed $offset, mixed $value): void {}

    /**
     * @throws \RuntimeException unconditionally because unset and null are different things, unlike SplFixedArray
     */
    public function offsetUnset(mixed $offset): void {}

    /**
     * Returns the offset of a value that is === $value, or returns null.
     */
    public function indexOf(mixed $value): ?int {}
    /**
     * Returns true if there exists a value === $value in this vector.
     */
    public function contains(mixed $value): bool {}

    /**
     * @implementation-alias Teds\LowMemoryVector::toArray
     */
    public function jsonSerialize(): array {}
}
