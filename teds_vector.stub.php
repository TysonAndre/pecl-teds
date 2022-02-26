<?php

/** @generate-class-entries */

namespace Teds;

/**
 * A Vector is a container of a sequence of values (with keys 0, 1, ...count($vector) - 1)
 * that can change in size.
 *
 * This is backed by a memory-efficient representation
 * (raw array of values) and provides fast access (compared to other objects in the Spl)
 * and constant amortized-time push/pop operations.
 *
 * Attempting to read or write values outside of the range of values with `*get`/`*set` methods will throw at runtime.
 */
final class Vector implements \IteratorAggregate, Sequence, \JsonSerializable
{
    /**
     * Construct a Vector from an iterable.
     *
     * The keys will be ignored, and values will be reindexed without gaps starting from 0
     */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator that will return the indexes and values of iterable until index >= count()
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of values in this Vector
     */
    public function count(): int {}
    /**
     * Returns whether this vector is empty (has a count of 0)
     */
    public function isEmpty(): bool {}
    /**
     * Returns the total capacity of this Vector.
     */
    public function capacity(): int {}
    /**
     * Reduces the Vector's capacity to its size, freeing any extra unused memory.
     */
    public function shrinkToFit(): void {}
    /**
     * If the current capacity is less than $capacity, raise it to capacity.
     * @throws \UnexpectedValueException if the new capacity is too large
     */
    public function reserve(int $capacity): void {}
    /**
     * Remove all elements from the array and free all reserved capacity.
     */
    public function clear(): void {}

    /**
     * If $size is greater than the current size, raise the size and fill the free space with $value
     * If $size is less than the current size, reduce the size and discard elements.
     */
    public function setSize(int $size, mixed $value = null): void {}

    /**
     * @implementation-alias Teds\Vector::toArray
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): Vector {}

    public function push(mixed ...$values): void {}
    public function pop(): mixed {}
    public function unshift(mixed ...$values): void {}
    public function shift(): mixed {}

    /** Read the first value or throws \UnderflowException */
    public function first(): mixed {}
    /** Read the last value or throws \UnderflowException */
    public function last(): mixed {}

    public function toArray(): array {}

    /**
     * @implementation-alias Teds\Vector::toArray
     * @override for Sequence::values()
     */
    public function values(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function get(int $offset): mixed {}
    public function set(int $offset, mixed $value): void {}
    public function insert(int $offset, mixed ...$values): void {}

    /**
     * Returns the value at (int)$offset.
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetGet(mixed $offset): mixed {}

    /**
     * Returns true if `0 <= (int)$offset && (int)$offset < $this->count()
     * AND the value of offsetGet is non-null.
     */
    public function offsetExists(mixed $offset): bool {}
    /**
     * Returns true if `is_int($offset) && 0 <= $offset && $offset < $this->count()
     */
    public function containsKey(mixed $offset): bool {}

    /**
     * Sets the value at offset (int)$offset to $value
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
     */
    public function offsetSet(mixed $offset, mixed $value): void {}

    /**
     * Removes the element at (int)$offset and reindexes the following elements of the vector.
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this vector
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
     * Returns a new Vector instance created from the return values of $callable($element)
     * being applied to each element of this vector.
     *
     * (at)param null|callable(mixed):mixed $callback
     */
    public function map(callable $callback): Vector {}
    /**
     * Returns the subset of elements of the Vector satisfying the predicate.
     *
     * If the value returned by the callback is truthy
     * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
     * this is treated as satisfying the predicate.
     *
     * (at)param null|callable(mixed):bool $callback
     */
    public function filter(?callable $callback = null): Vector {}

    /**
     * @implementation-alias Teds\Vector::toArray
     */
    public function jsonSerialize(): array {}
}
