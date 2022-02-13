<?php

/** @generate-class-entries */

namespace Teds;

/**
 * This exposes the same API as a Vector, but with a more memory-efficient representation.
 *
 * Benefits:
 *
 * - For collections of exclusively int, exclusively floats, or exclusively null/true/false, this uses less memory when serialized compared to vectors/arrays.
 *
 *   Note that adding other types will make this use as much memory as a Vector, e.g. adding any non-float (including int) to a collection of floats.
 * - Has faster checks for contains/indexOf (if values can have an optimized representation)
 * - Has faster garbage collection (if values can have an optimized representation due to int/float/bool/null not needing reference counting).
 *
 * Drawbacks:
 * - Slightly more overhead when types aren't specialized.
 *
 * In 64-bit builds, the following types are supported, with the following amounts of memory (plus constant overhead to represent the LowMemoryVector itself, and extra capacity for growing the LowMemoryVector):
 *
 * 1. null/bool : 1 byte per value.
 *
 *    (In serialize(), this is even less, adding 1 to 2 bits per value (2 bits if null is included)).
 * 2. signed 8-bit value (-128..127): 1 byte per value.
 * 3. signed 16-bit value: 2 bytes per value.
 * 4. signed 32-bit value: 4 bytes per value.
 * 5. signed 64-bit value: 8 bytes per value.
 * 6. signed PHP `float`:  8 bytes per value.
 * 7. `mixed` or combinations of the above: 16 bytes per value.
 *
 * In comparison, in 64-bit builds of PHP, PHP's arrays take at least 16 bytes per value in php 8.2, and at least 32 bytes per value before php 8.1, at the time of writing.
 */
final class LowMemoryVector implements \IteratorAggregate, Sequence, \JsonSerializable
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

    /** @psalm-return list<mixed> */
    public function toArray(): array {}
    /** @implementation-alias Teds\LowMemoryVector::toArray */
    public function values(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function get(int $offset): mixed {}
    public function set(int $offset, mixed $value): void {}

    /**
     * Returns the value at (int)$offset.
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetGet(mixed $offset): mixed {}

    /**
     * Returns true if `0 <= (int)$offset && (int)$offset < $this->count()
     * AND the value of offsetGet($offset) is not null.
     */
    public function offsetExists(mixed $offset): bool {}
    /**
     * Returns true if `is_int($offset) && 0 <= $offset && $offset < $this->count().
     */
    public function containsKey(mixed $offset): bool {}

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
