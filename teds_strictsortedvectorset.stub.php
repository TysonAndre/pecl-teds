<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Set using a stable sort, which can contain any value type.
 * Iteration will return values in the order they would be sorted by `Teds\stable_compare`, ascending.
 * This is a set where entries for keys of any type can be created if `Teds\stable_compare($key1, $key2) != 0`,
 * otherwise the previous entry is deleted.
 *
 * This is backed by a sorted array of unique values, with the following tradeoffs.
 * + More efficient memory usage than a regular StrictTreeSet.
 * + Faster unserialization when serialized data is sorted
 * + Good performance when writes are infrequent. (e.g. reading a rarely changed object from a cache such as memcached)
 * - Linear time needed for each write due to using insertion sort.
 *
 * @alias Teds\StableSortedListMap
 * (Deprecated alias - do not use)
 * @todo add helper methods for finding nth element, finding position of value in the set, etc.
 */
final class StrictSortedVectorSet implements \IteratorAggregate, Set, \JsonSerializable
{
    /** Construct the StrictSortedVectorSet from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator over the values of the StrictSortedVectorSet.
     * key() and value() both return the same value.
     * Elements are iterated over in sorted order.
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of values in the StrictSortedVectorSet.
     * @implementation-alias Teds\Vector::count
     */
    public function count(): int {}
    /**
     * Returns true if there are 0 values in the StrictSortedVectorSet.
     * @implementation-alias Teds\Vector::isEmpty
     */
    public function isEmpty(): bool {}
    /**
     * Removes all elements from the StrictSortedVectorSet.
     * @implementation-alias Teds\Vector::clear
     */
    public function clear(): void {}

    /**
     * Returns a list of the values in order of insertion.
     * @implementation-alias Teds\Vector::toArray
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StrictSortedVectorSet from values of the array. */
    public static function __set_state(array $array): StrictSortedVectorSet {}

    /**
     * Returns a list of the unique values in order of insertion.
     * @implementation-alias Teds\Vector::toArray
     */
    public function values(): array {}

    /** Returns keys mapped to themselves, like iterator_to_array would return. */
    public function toArray(): array {}

    /**
     * Returns the first value, throws if empty.
     * @implementation-alias Teds\Vector::first
     */
    public function first(): mixed {}
    /** @implementation-alias Teds\Vector::first */
    public function bottom(): mixed {}

    /**
     * Returns the last value, throws if empty
     * @implementation-alias Teds\Vector::last
     */
    public function last(): mixed {}
    /**
     * @implementation-alias Teds\Vector::last
     */
    public function top(): mixed {}

    /**
     * Pops a value from the end of the StrictSortedVectorSet.
     * @throws \UnderflowException if the StrictSortedVectorSet is empty
     * @implementation-alias Teds\Vector::pop
     */
    public function pop(): mixed {}
    /**
     * Shifts a value from the front of the StrictSortedVectorSet
     * @throws \UnderflowException if the StrictSortedVectorSet is empty
     * @implementation-alias Teds\Vector::shift
     */
    public function shift(): mixed {}
    /**
     * Returns true if $value exists in this StrictSortedVectorSet, false otherwise.
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if $value was added to this StrictSortedVectorSet and was not previously in this StrictSortedVectorSet.
     */
    public function add(mixed $value): bool {}

    /**
     * Returns true if $value was found in this StrictSortedVectorSet before being removed from this StrictSortedVectorSet.
     */
    public function remove(mixed $value): bool {}

    /**
     * Returns [v1, v2, ...]
     * @implementation-alias Teds\Vector::toArray
     */
    public function jsonSerialize(): array {}
}
