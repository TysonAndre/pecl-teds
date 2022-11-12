<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */
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
 * + More efficient memory usage than a regular StrictTreeMap.
 * + Faster unserialization when serialized data is sorted
 * + Good performance when writes are infrequent. (e.g. reading a rarely changed object from a cache such as memcached)
 * - Linear time needed for each write due to using insertion sort.
 *
 * @alias Teds\StableSortedListMap
 * (Deprecated alias - do not use)
 * @todo add helper methods for finding nth element, finding position of value in the set, etc.
 */
final class StrictSortedVectorMap implements \IteratorAggregate, Map, \JsonSerializable
{
    /** Construct the StrictSortedVectorMap from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator over the keys and values of the StrictSortedVectorMap
     * @implementation-alias Teds\MutableIterable::getIterator
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of elements in the StrictSortedVectorMap.
     * @implementation-alias Teds\MutableIterable::count
     */
    public function count(): int {}
    /**
     * Returns true if there are 0 elements in the StrictSortedVectorMap.
     * @implementation-alias Teds\MutableIterable::isEmpty
     */
    public function isEmpty(): bool {}
    /**
     * Removes all elements from the StrictSortedVectorMap.
     * @implementation-alias Teds\MutableIterable::clear
     */
    public function clear(): void {}
    /**
     * Converts this to [[key1, value1], [key2, value2]]
     * @implementation-alias Teds\MutableIterable::toPairs
     */
    public function toPairs(): array {}
    /** Create this from [[key1, value1], [key2, value2]] */
    public static function fromPairs(iterable $pairs): StrictSortedVectorMap {}

    /**
     * @implementation-alias Teds\MutableIterable::__serialize
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StrictSortedVectorMap from the keys and values of the array ([[key1, value1], [key2, value2]]) */
    public static function __set_state(array $array): StrictSortedVectorMap {}

    /**
     * @implementation-alias Teds\MutableIterable::toArray
     */
    public function toArray(): array {}
    /**
     * Returns a list of the values sorted by the corresponding key.
     * @implementation-alias Teds\MutableIterable::values
     */
    public function values(): array {}

    /**
     * Returns a list of the keys sorted by key
     * @implementation-alias Teds\MutableIterable::keys
     */
    public function keys(): array {}

    // first/last matches array_key_first/last, bottom/top matches use for SplDoublyLinkedList.

    /** Returns the first value, throws if empty. */
    public function first(): mixed {}
    /** @implementation-alias Teds\StrictSortedVectorMap::first */
    public function bottom(): mixed {}

    /** Returns the first key, throws if empty. */
    public function firstKey(): mixed {}
    /** @implementation-alias Teds\StrictSortedVectorMap::firstKey */
    public function bottomKey(): mixed {}

    /** Returns the last value, throws if empty */
    public function last(): mixed {}
    /** @implementation-alias Teds\StrictSortedVectorMap::last */
    public function top(): mixed {}

    /** Returns the last key, throws if empty */
    public function lastKey(): mixed {}
    /** @implementation-alias Teds\StrictSortedVectorMap::lastKey */
    public function topKey(): mixed {}

    /**
     * Pops the [key, value] entry from the end of the StrictSortedVectorMap.
     * @throws \UnderflowException if the StrictSortedVectorMap is empty
     * @implementation-alias Teds\MutableIterable::pop
     */
    public function pop(): array {}
    /**
     * Removes and returns the [key, value] entry from the front of the StrictSortedVectorMap
     * @throws \UnderflowException if the StrictSortedVectorMap is empty
     */
    public function popFront(): mixed {}
    /** @implementation-alias Teds\StrictSortedVectorMap::popFront */
    public function shift(): mixed {}
    /**
     * Returns the value for the given key.
     * @throws \UnderflowException if the StrictSortedVectorMap is empty
     * @see StrictSortedVectorMap::get
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
     * Returns true if there exists a value === $value in this StrictSortedVectorMap.
     * @implementation-alias Teds\MutableIterable::contains
     */
    public function containsValue(mixed $value): bool {}
    /**
     * Returns true if there exists a value === $value in this StrictSortedVectorMap.
     * @implementation-alias Teds\MutableIterable::contains
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if there exists a key === $key in this StrictSortedVectorMap.
     * Unlike offsetExists, this returns true even if the corresponding value is null.
     */
    public function containsKey(mixed $key): bool {}

    // TODO: Add keyAt/valueAt/indexOfKey

    /**
     * Returns [[key1, value1], [key2, value2]]
     * @implementation-alias Teds\MutableIterable::toPairs
     */
    public function jsonSerialize(): array {}
}
