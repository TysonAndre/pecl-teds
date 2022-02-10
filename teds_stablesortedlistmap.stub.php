<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Map using a stable sort, which can contain any key type.
 * Iteration will return elements in the order their keys would be sorted by `Teds\stable_compare`, ascending.
 * This is a map where entries for keys of any type can be created if `Teds\stable_compare($key1, $key2) != 0`,
 * otherwise the previous entry is deleted.
 *
 * This is backed by a sorted list of entries with unique keys.
 * This has the following properties:
 *
 * - Memory efficient
 * - Efficient at unserialization when entries are in sorted order.
 * - Slow at writes (linear time for each write) due to using insertion sort
 */
final class StableSortedListMap implements \IteratorAggregate, Collection, \JsonSerializable
{
    /** Construct the StableSortedListMap from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /* Returns an iterator over the keys and values of the StableSortedListMap */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of elements in the StableSortedListMap. */
    public function count(): int {}
    /** Returns true if there are 0 elements in the StableSortedListMap. */
    public function isEmpty(): bool {}
    /** Removes all elements from the StableSortedListMap. */
    public function clear(): void {}
    /** Converts this to [[key1, value1], [key2, value2]] */
    public function toPairs(): array {}
    /** Create this from [[key1, value1], [key2, value2]] */
    public static function fromPairs(iterable $pairs): StableSortedListMap {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StableSortedListMap from the keys and values of the array ([[key1, value1], [key2, value2]]) */
    public static function __set_state(array $array): StableSortedListMap {}

    public function toArray(): array {}
    /** Returns a list of the values in order of insertion. */
    public function values(): array {}

    /** Returns a list of the keys in order of insertion. */
    public function keys(): array {}

    // bottom/top matches use for SplDoublyLinkedList.

    /** Returns the first value, throws if empty. */
    public function bottom(): mixed {}

    /** Returns the first key, throws if empty. */
    public function bottomKey(): mixed {}

    /** Returns the last value, throws if empty */
    public function top(): mixed {}

    /** Returns the last key, throws if empty */
    public function topKey(): mixed {}

    /**
     * Pops the [key, value] entry from the end of the SortedStrictSet.
     * @throws \UnderflowException if the SortedStrictSet is empty
     */
    public function pop(): array {}
    /**
     * Shifts the [key, value] entry from the front of the SortedStrictSet
     * @throws \UnderflowException if the SortedStrictSet is empty
     */
    public function shift(): mixed {}
    /**
     * Returns the value for the given key.
     * @throws \UnderflowException if the SortedStrictSet is empty
     * @see StableSortedListMap::get
     */
    public function offsetGet(mixed $key): mixed {}
    /**
     * Returns true if the value for the key exists and is not null
     */
    public function offsetExists(mixed $key): bool {}
    /**
     * Sets the value at the given key.
     */
    public function offsetSet(mixed $key, mixed $value): void {}
    /**
     * Unsets the value for key $key. Does nothing if the key is not found.
     */
    public function offsetUnset(mixed $key): void {}

    /**
     * Returns the value at $key.
     * @throws \OutOfBoundsException if $key is not found and $default was not provided.
     */
    public function get(mixed $key, mixed $default = null): mixed {}

    /**
     * Returns true if there exists a value === $value in this StableSortedListMap.
     */
    public function containsValue(mixed $value): bool {}

    /**
     * Returns true if there exists a key === $key in this StableSortedListMap.
     * Unlike offsetExists, this returns true even if the corresponding value is null.
     */
    public function containsKey(mixed $value): bool {}

    // TODO: Add keyAt/valueAt/indexOfKey

    /**
     * Returns [[key1, value1], [key2, value2]]
     * @implementation-alias Teds\StableSortedListMap::toPairs
     */
    public function jsonSerialize(): array {}
}
