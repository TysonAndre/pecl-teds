<?php

/** @generate-class-entries */

namespace Teds;

/**
 * Collection is a common interface for an object with values that can be iterated over and counted,
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 *
 * Note: JsonSerializable was not included here because it
 * 1. Causes issues or inefficiencies with some ways of implementing data structures internally.
 * 2. Forces the result to be represented as []
 * 3. Forces polyfills to implement JsonSerializable as well, even when it would be less efficient
 *
 * @alias Teds\Values
 * (This alias will be removed in a future major release. Do not use it.)
 */
interface Collection extends \Traversable, \Countable {
    /** @psalm-return list<values> the list of values in the collection */
    public function values(): array {}

    /**
     * Returns a list or associative array,
     * typically attempting to cast keys to array key types (int/string) to insert elements of the collection into the array, or throwing.
     *
     * When this is impossible for the class in general,
     * this returns an array with representations of key/value entries of the Collection.
     */
    public function toArray(): array {}

    // TODO: Provide a trait that provides default implementations for these methods?

    /** Returns true if count() would be 0 */
    public function isEmpty(): bool {}

    /** Returns true if this contains a value identical to $value */
    public function contains(mixed $value): bool {}
}

/**
 * This represents a Collection that can be used like a list without gaps.
 * E.g. get()/set() will work for is_int($offset) && 0 <= $offset and $offset < $list->count().
 *
 * NOTE: List is a reserved keyword in php and cannot be used as an identifier, e.g. `list($x) = [1]`.
 */
interface Sequence extends Collection, \ArrayAccess {
    public function get(int $offset): mixed {}
    public function set(int $offset, mixed $value): void {}

    public function push(mixed ...$values): void {}
    public function pop(): mixed {}
    public function unshift(mixed ...$values): void {}
    public function shift(): mixed {}
}

/**
 * A Map is a type of Collection mapping unique keys to values.
 *
 * Implementations should either coerce unsupported key types or throw TypeError when using keys.
 */
interface Map extends Collection, \ArrayAccess {
    /**
     * Returns true if there exists a key identical to $key according to the semantics of the implementing collection.
     * Typically, this is close to the definition of `===`, but may be stricter or weaker in some implementations, e.g. for NAN, negative zero, etc.
     *
     * containsKey differs from offsetExists, where implementations of offsetExists usually return false if the key was found but the corresponding value was null.
     * (This is analogous to the difference between array_key_exists($key, $array) and isset($array[$key]))
     */
    public function containsKey(mixed $value): bool {}
}

/**
 * A Set is a type of Collection representing a set of unique values.
 */
interface Set extends Collection {
    /**
     * Returns true if $value was added to this Set and was not previously in this Set.
     */
    public function add(mixed $value): bool {}

    /**
     * Returns true if $value was found in this Set before being removed from this Set.
     */
    public function remove(mixed $value): bool {}
}
