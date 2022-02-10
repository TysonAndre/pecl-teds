<?php

/** @generate-class-entries */

namespace Teds;

/**
 * Values is a common interface for an object with values that can be iterated over and counted,
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 */
interface Values extends \Traversable, \Countable {
    /** @psalm-return list<values> */
    public function values(): array {}

    /** Returns true if count() would be 0 */
    public function isEmpty(): bool {}

    /** Returns true if this contains a value identical to $value */
    public function contains(mixed $value): bool {}
}

/**
 * A sequence represents a list of values that can be iterated over and counted,
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 *
 * Note: JsonSerializable was not included here because
 * 1. Causes issues or inefficiencies with some ways of implementing data structures internally.
 * 2. Forces the result to be represented as []
 * 3. Forces polyfills to implement JsonSerializable as well, even when it would be less efficient
 */
interface Collection extends Values, \ArrayAccess {
    /**
     * Returns a list or associative array,
     * typically attempting to cast keys to array key types (int/string) to insert elements of the collection into the array, or throwing.
     *
     * When this is impossible for the class in general,
     * this returns an array with representations of key/value entries of the Collection.
     */
    public function toArray(): array {}

    // clear will throw UnsupportedException on immutables.
    /**
     * Returns true if there exists a key === $key in this Collection.
     */
    public function containsKey(mixed $value): bool {}
}

/**
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 *
 * Note: JsonSerializable was not included here because
 * 1. Causes issues or inefficiencies with some ways of implementing data structures internally.
 * 2. Forces the result to be represented as []
 * 3. Forces polyfills to implement JsonSerializable as well, even when it would be less efficient
 *
 * NOTE: List is a reserved keyword in php and cannot be used as an identifier, e.g. `list($x) = [1]`.
 */
interface ListInterface extends Collection {
    public function get(int $key): mixed {}
    public function set(int $offset, mixed $value): void {}

    public function push(mixed ...$values): void {}
    public function pop(): mixed {}
}
