<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Set using a stable sort, which can contain any value type.
 * Iteration will return values in the order they would be sorted by `Teds\stable_compare`, ascending.
 * This is a set where entries for keys of any type can be created if `Teds\stable_compare($key1, $key2) != 0`,
 * otherwise the previous entry is deleted.
 *
 * This is backed by a sorted array of unique values, with the following tradeoffs.
 * - Very efficient memory usage.
 * - Very fast unserialization when serialized data is sorted
 * - Good performance when writes are infrequent.
 * - Linear time needed for each write due to using insertion sort.
 *
 * TODO: Rename to ListBasedSortedStrictSet
 *
 * TODO: Support indexOf methods
 */
final class StableSortedListSet implements \IteratorAggregate, Set, \JsonSerializable
{
    /** Construct the StableSortedListSet from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator over the values of the StableSortedListSet.
     * key() and value() both return the same value.
     * Elements are iterated over in sorted order.
     */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of values in the StableSortedListSet. */
    public function count(): int {}
    /** Returns true if there are 0 values in the StableSortedListSet. */
    public function isEmpty(): bool {}
    /** Removes all elements from the StableSortedListSet. */
    public function clear(): void {}

    /**
     * Returns a list of the values in order of insertion.
     * @implementation-alias Teds\StableSortedListSet::values
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StableSortedListSet from values of the array. */
    public static function __set_state(array $array): StableSortedListSet {}

    /** Returns a list of the unique values in order of insertion. */
    public function values(): array {}

    /** Returns keys mapped to themselves, like iterator_to_array would return. */
    public function toArray(): array {}

    /** Returns the first value, throws if empty. */
    public function bottom(): mixed {}

    /** Returns the last value, throws if empty */
    public function top(): mixed {}

    /**
     * Pops a value from the end of the StableSortedListSet.
     * @throws \UnderflowException if the StableSortedListSet is empty
     */
    public function pop(): mixed {}
    /**
     * Shifts a value from the front of the StableSortedListSet
     * @throws \UnderflowException if the StableSortedListSet is empty
     */
    public function shift(): mixed {}
    /**
     * Returns true if $value exists in this StableSortedListSet, false otherwise.
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if $value was added to this StableSortedListSet and was not previously in this StableSortedListSet.
     */
    public function add(mixed $value): bool {}

    /**
     * Returns true if $value was found in this StableSortedListSet before being removed from this StableSortedListSet.
     */
    public function remove(mixed $value): bool {}

    /**
     * Returns [v1, v2, ...]
     * @implementation-alias Teds\StableSortedListSet::values
     */
    public function jsonSerialize(): array {}
}
