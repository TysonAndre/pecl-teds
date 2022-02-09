<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * Strict set which can contain any value type, based on a hash table.
 * **Iteration is a work in progress.** Iteration will not work as expected if elements are removed during iteration.
 *
 * This is a set where values of any type can be inserted if they are `!==` to other keys and have different values of strict_hash.
 * This uses `Teds\strict_hash`.
 *
 * ArrayAccess is not implemented because `$x[$item] = $value` is meaningless
 * and there could be multiple desired meetings of `$value = $x[$item];`
 */
final class StrictSet implements \IteratorAggregate, Values, \JsonSerializable
{
    /** Construct the StrictSet from the values of the Traversable/array, ignoring keys. */
    public function __construct(iterable $iterator = []) {}
    /** Returns an iterator over the values of the StrictSet. key() and value() both return the same value. */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of values in the StrictSet. */
    public function count(): int {}
    /** Returns true if there are 0 values in the StrictSet. */
    public function isEmpty(): bool {}
    /** Removes all elements from the StrictSet. */
    public function clear(): void {}

    /**
     * Returns a list of the values in order of insertion.
     * @implementation-alias Teds\StrictSet::values
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the StrictSet from values of the array. */
    public static function __set_state(array $array): StrictSet {}

    /** Returns a list of the unique values in order of insertion. */
    public function values(): array {}

    /**
     * Returns true if $value exists in this StrictSet, false otherwise.
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if $value was added to this StrictSet and was not previously in this StrictSet.
     */
    public function add(mixed $value): bool {}

    /**
     * Returns true if $value was found in this StrictSet before being removed from this StrictSet.
     */
    public function remove(mixed $value): bool {}

    /**
     * Returns [v1, v2, ...]
     * @implementation-alias Teds\StrictSet::values
     */
    public function jsonSerialize(): array {}
}
