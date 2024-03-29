<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Map using a stable sort for keys, which can contain any key type.
 * Iteration will return elements in the order their keys would be sorted by `Teds\stable_compare`, from smallest to largest.
 *
 * This is a map where entries for keys of any type can be created if `Teds\stable_compare($key1, $key2) != 0`,
 * otherwise the value for the previous entry is replaced.
 *
 * This is backed by a balanced red-black tree to ensure that insertions/removals/lookups take logarithmic time in the worst case.
 *
 * @alias Teds\StableSortedMap
 * (This alias will be removed in a future Teds release. Do not use it.)
 */
final class StrictTreeMap implements \IteratorAggregate, Map, \JsonSerializable
{
    /** Construct the StrictTreeMap from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /* Returns an iterator over the keys and values of the StrictTreeMap */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of elements in the StrictTreeMap. */
    public function count(): int {}
    /** Returns true if there are 0 elements in the StrictTreeMap. */
    public function isEmpty(): bool {}
    /** Removes all elements from the StrictTreeMap. */
    public function clear(): void {}
    /** Converts this to [[key1, value1], [key2, value2]] */
    public function toPairs(): array {}
    /** Implementation of Collection::toArray, same result as iterator_to_array($this) */
    public function toArray(): array {}
    /** Create this from [[key1, value1], [key2, value2]] */
    public static function fromPairs(iterable $pairs): StrictTreeMap {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StrictTreeMap from the keys and values of the array ([[key1, value1], [key2, value2]]) */
    public static function __set_state(array $array): StrictTreeMap {}

    /** Returns a list of the values in order of insertion. */
    public function values(): array {}

    /** Returns a list of the keys in order of insertion. */
    public function keys(): array {}

    // bottom/top matches use for SplDoublyLinkedList.

    /** Returns the first value, throws if empty. */
    public function first(): mixed {}
    /** @implementation-alias Teds\StrictTreeMap::first */
    public function bottom(): mixed {}

    /** Returns the first key, throws if empty. */
    public function firstKey(): mixed {}
    /** @implementation-alias Teds\StrictTreeMap::firstKey */
    public function bottomKey(): mixed {}

    /** Returns the last value, throws if empty */
    public function last(): mixed {}
    /** @implementation-alias Teds\StrictTreeMap::last */
    public function top(): mixed {}

    /** Returns the last key, throws if empty */
    public function lastKey(): mixed {}
    /** @implementation-alias Teds\StrictTreeMap::lastKey */
    public function topKey(): mixed {}

    /**
     * Pops the [key, value] entry from the end of the StrictTreeMap.
     * @throws \UnderflowException if the StrictTreeMap is empty
     */
    public function pop(): array {}
    /**
     * Removes and returns the [key, value] entry from the front of the StrictTreeMap
     * @throws \UnderflowException if the StrictTreeMap is empty
     */
    public function popFront(): mixed {}
    /** @implementation-alias Teds\StrictTreeMap::popFront */
    public function shift(): mixed {}
    /**
     * Returns the value for the given key.
     * @throws \UnderflowException if the StrictTreeMap is empty
     * @see StrictTreeMap::get
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
     * Returns true if there exists a value === $value in this StrictTreeMap.
     * @implementation-alias Teds\StrictTreeMap::contains
     * @deprecated
     */
    public function containsValue(mixed $value): bool {}
    /**
     * Returns true if there exists a value === $value in this StrictTreeMap.
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if there exists a key where `stable_compare($key, $other) === 0` in this StrictTreeMap.
     * Unlike offsetExists, this returns true even if the corresponding value is null.
     *
     * Note that this is not exactly `===` but close: NAN !== NAN but this will return true for NAN.
     */
    public function containsKey(mixed $key): bool {}

    /**
     * Returns [[key1, value1], [key2, value2]]
     * @implementation-alias Teds\StrictTreeMap::toPairs
     */
    public function jsonSerialize(): array {}

    /**
     * @internal used for unit tests. Do not use.
     */
    public function debugGetTreeRepresentation(): array {}

    /**
     * @internal used for unit tests. Do not use.
     */
    public function debugIsBalanced(): bool {}
}
