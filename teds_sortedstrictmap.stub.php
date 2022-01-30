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
 * **This is a work in progress.** Iteration will not work as expected if elements are removed during iteration,
 * and this is backed by a raw array (using insertion sort) instead of a balanced binary tree,
 * meaning that insertion/removals are inefficient (linear time to move values) for large maps.
 */
final class SortedStrictMap implements \IteratorAggregate, \Countable, \JsonSerializable, \ArrayAccess
{
    /** Construct the SortedStrictMap from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /* Returns an iterator over the keys and values of the SortedStrictMap */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of elements in the SortedStrictMap. */
    public function count(): int {}
    /** Returns true if there are 0 elements in the SortedStrictMap. */
    public function isEmpty(): bool {}
    /** Removes all elements from the SortedStrictMap. */
    public function clear(): void {}
    /** Converts this to [[key1, value1], [key2, value2]] */
    public function toPairs(): array {}
    /** Create this from [[key1, value1], [key2, value2]] */
    public static function fromPairs(iterable $pairs): SortedStrictMap {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the SortedStrictMap from the keys and values of the array ([[key1, value1], [key2, value2]]) */
    public static function __set_state(array $array): SortedStrictMap {}

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
     * @see SortedStrictMap::get
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
     * Returns true if there exists a value === $value in this SortedStrictMap.
     */
    public function containsValue(mixed $value): bool {}

    /**
     * Returns true if there exists a key === $key in this SortedStrictMap.
     * Unlike offsetExists, this returns true even if the corresponding value is null.
     */
    public function containsKey(mixed $value): bool {}

    /**
     * Returns [[key1, value1], [key2, value2]]
     * @implementation-alias Teds\SortedStrictMap::toPairs
     */
    public function jsonSerialize(): array {}

    /**
     * @internal used for unit tests. Do not use.
     */
    public function debugGetTreeRepresentation(): array {}
}
