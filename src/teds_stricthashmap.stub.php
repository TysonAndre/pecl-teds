<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Strict map which can contain any key type.
 * **This is a work in progress - the api may change in 0.(x+1)/major versions.** Iteration will not work as expected if elements are removed during iteration, and hash lookups haven't been implemented yet, so this is inefficient for large maps (scans the entire map).
 *
 * This is a map where entries for keys of any type can be inserted if they are `!==` to other keys.
 * This uses `Teds\strict_hash`.
 */
final class StrictHashMap implements \IteratorAggregate, Map, \JsonSerializable
{
    /** Construct the StrictHashMap from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /* Returns an iterator over the keys and values of the StrictHashMap */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of elements in the StrictHashMap. */
    public function count(): int {}
    /** Returns true if there are 0 elements in the StrictHashMap. */
    public function isEmpty(): bool {}
    /** Removes all elements from the StrictHashMap. */
    public function clear(): void {}
    /** Converts this to [[key1, value1], [key2, value2]] */
    public function toPairs(): array {}
    /** Create this from [[key1, value1], [key2, value2]] */
    public static function fromPairs(iterable $pairs): StrictHashMap {}

    /** Returns [key1, value1, key2, value2] */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StrictHashMap from the keys and values of the array ([[key1, value1], [key2, value2]]) */
    public static function __set_state(array $array): StrictHashMap {}

    public function toArray(): array {}
    /** Returns a list of the values in order of insertion. */
    public function values(): array {}

    /** Returns a list of the keys in order of insertion. */
    public function keys(): array {}

    /**
     * Returns the value for the given key.
     * @throws \OutOfBoundsException if $key is not found and $default was not provided.
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
     * Returns true if there exists a value === $value in this StrictHashMap.
     * @implementation-alias Teds\StrictHashMap::contains
     * @deprecated
     */
    public function containsValue(mixed $value): bool {}
    /**
     * Returns true if there exists a value === $value in this StrictHashMap.
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if there exists a key === $key in this StrictHashMap.
     */
    public function containsKey(mixed $key): bool {}

    /**
     * Returns the value at $key.
     * @throws \OutOfBoundsException if $key is not found and $default was not provided.
     */
    public function get(mixed $key, mixed $default = null): mixed {}

    /**
     * Returns [[key1, value1], [key2, value2]]
     * @implementation-alias Teds\StrictHashMap::toPairs
     */
    public function jsonSerialize(): array {}
}
