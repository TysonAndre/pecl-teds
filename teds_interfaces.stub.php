<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */

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
     * this throws.
     */
    public function toArray(): array {}

    // TODO: Provide a trait that provides default implementations for these methods?

    /** Returns true if count() would be 0 */
    public function isEmpty(): bool {}

    /** Returns true if this contains a value identical to $value (for the definition of value equality used by the implementation, which may be stricter or weaker than `===`) */
    public function contains(mixed $value): bool {}

    /** Removes all elements from this Collection or throws. */
    public function clear(): void {}
}

/**
 * This represents a Collection that can be used like a list without gaps.
 * E.g. get()/set() will work for is_int($offset) && 0 <= $offset and $offset < $list->count().
 *
 * @implementation-notes
 * NOTE: List is a reserved keyword in php and cannot be used as an identifier, e.g. `list($x) = [1]`.
 * List is also used as a non-standard type in phpdoc for arrays where array_is_list() is true.
 *
 * NOTE: Many methods of Sequence throw \OutOfBoundsException.
 * https://www.php.net/manual/en/class.outofboundsexception.php is described as an exception that "represents errors that cannot be detected at compile time."
 * Since the length of a Sequence (e.g. Vector, Deque) is often not known at compile time and the code in question may be accepting `Sequence $sequence`,
 * throwing `OutOfBoundsException extends RuntimeException` seems more appropriate than `OutOfRangeException extends LogicException`
 */
interface Sequence extends Collection, \ArrayAccess {
    /**
     * Returns the value at $offset.
     * Type safe alternative to offsetGet.
     *
     * @throws \OutOfBoundsException if $offset < 0 or $offset >= count
     */
    public function get(int $offset): mixed {}
    /**
     * Sets the value at $offset to $value.
     * Type safe alternative to offsetSet.
     *
     * @throws \OutOfBoundsException if $offset < 0 or $offset >= count
     */
    public function set(int $offset, mixed $value): void {}
    /**
     * Inserts 0 or more values into $offset and move subsequent values out of the way.
     *
     * @throws \OutOfBoundsException if $offset < 0 or $offset > count
     */
    public function insert(int $offset, mixed ...$values): void {}

    /**
     * Append 0 or more values to the end of the Sequence.
     */
    public function push(mixed ...$values): void {}
    /**
     * Remove the last value and returns its value.
     * @throws \UnderflowException if there are no more elements.
     */
    public function pop(): mixed {}
    /**
     * Prepend 0 or more values to the start of the sequence.
     */
    public function pushFront(mixed ...$values): void {}
    /**
     * Removes the first value and returns its value.
     * @throws \UnderflowException if there are no more elements.
     */
    public function popFront(): mixed {}
    /**
     * Returns the first value (at offset 0)
     * @throws \UnderflowException if there are no elements.
     */
    public function first(): mixed {}
    /**
     * Returns the first value (at offset count() - 1)
     * @throws \UnderflowException if there are no elements.
     */
    public function last(): mixed {}
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
     * (The difference between containsKey and offsetExists is analogous to the difference between array_key_exists($key, $array) and isset($array[$key]))
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
