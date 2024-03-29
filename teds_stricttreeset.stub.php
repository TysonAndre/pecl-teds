<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Set using a stable sort, which can contain any value type.
 * Iteration will return values in the order they would be sorted by `Teds\stable_compare`, from smallest to largest.
 *
 * This is a set where entries for keys of any type can be created if `Teds\stable_compare($key1, $key2) != 0`,
 * otherwise the previous entry is kept.
 *
 * This is backed by a balanced binary tree to ensure that insertions/removals/lookups take logarithmic time in the worst case.
 *
 * @alias Teds\StableSortedSet
 * (This alias will be removed in a future major version. Do not use it.)
 */
final class StrictTreeSet implements \IteratorAggregate, Set, \JsonSerializable
{
    /** Construct the StrictTreeSet from the keys and values of the Traversable/array. */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator over the values of the StrictTreeSet.
     * key() and value() both return the same value.
     * Elements are iterated over in sorted order.
     */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of values in the StrictTreeSet. */
    public function count(): int {}
    /** Returns true if there are 0 values in the StrictTreeSet. */
    public function isEmpty(): bool {}
    /** Removes all elements from the StrictTreeSet. */
    public function clear(): void {}

    /**
     * Returns a list of the values in order of insertion.
     * @implementation-alias Teds\StrictTreeSet::values
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StrictTreeSet from values of the array. */
    public static function __set_state(array $array): StrictTreeSet {}

    /** Returns a list of the unique values in order of insertion. */
    public function values(): array {}

    /**
     * Returns array created by mapping values to themselves, like iterator_to_array.
     * Throws or emits notices if values cannot be converted to array keys.
     */
    public function toArray(): array {}

    /** Returns the first value, throws if empty. */
    public function first(): mixed {}
    /** @implementation-alias Teds\StrictTreeSet::first */
    public function bottom(): mixed {}

    /** Returns the last value, throws if empty */
    public function last(): mixed {}
    /** @implementation-alias Teds\StrictTreeSet::last */
    public function top(): mixed {}

    /**
     * Pops a value from the end of the StrictTreeSet.
     * @throws \UnderflowException if the StrictTreeSet is empty
     */
    public function pop(): mixed {}
    /**
     * Removes and returns a value from the front of the StrictTreeSet
     * @throws \UnderflowException if the StrictTreeSet is empty
     */
    public function popFront(): mixed {}
    /** @implementation-alias Teds\StrictTreeSet::popFront */
    public function shift(): mixed {}

    /**
     * Returns true if $value exists in this StrictTreeSet, false otherwise.
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if $value was added to this StrictTreeSet and was not previously in this StrictTreeSet.
     */
    public function add(mixed $value): bool {}

    /**
     * Returns true if $value was found in this StrictTreeSet before being removed from this StrictTreeSet.
     */
    public function remove(mixed $value): bool {}

    /**
     * Returns [v1, v2, ...]
     * @implementation-alias Teds\StrictTreeSet::values
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
