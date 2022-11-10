<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */

namespace Teds;

/**
 * An immutable set of unique strings sorted by strcmp.
 * This is designed for fast unserialization/unserialization with low memory usage, and relatively fast (logarithmic) lookup time.
 * (e.g. for sets that are rarely modified but frequently unserialized and read from)
 *
 * See benchmarks/benchmark_string_set.php
 */
final class ImmutableSortedStringSet implements \IteratorAggregate, Set, \JsonSerializable
{
    /**
     * Construct an ImmutableSortedStringSet from an iterable of strings, deduplicating and sorting the result.
     *
     * The keys will be ignored, and values will be sorted and reindexed.
     * @psalm-param iterator<string> $iterator
     */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator that will return the indexes and values of this ImmutableSortedStringSet
     */
    public function getIterator(): \InternalIterator {}
    /**
     * Returns the number of values in this ImmutableSortedStringSet
     */
    public function count(): int {}
    /**
     * Returns whether this set of strings is empty (has a count of 0)
     */
    public function isEmpty(): bool {}

    /**
     * Returns an array containing a little-endian binary representation of the number of strings, lengths of strings, and sorted strings.
     */
    public function __serialize(): array {}
    /**
     * Unserializes the ImmutableSortedStringSet from an array containing a little-endian binary representation of the number of strings, lengths of strings, and sorted strings.
     */
    public function __unserialize(array $data): void {}

    /**
     * Returns a string containing a little-endian binary representation of the number of strings, lengths of strings, and sorted strings.
     */
    public function serialize(): string {}
    /**
     * Returns an ImmutableSortedStringSet created from a little-endian binary representation of the number of strings, lengths of strings, and sorted strings.
     */
    public static function unserialize(string $data): ImmutableSortedStringSet {}

    /**
     * Create an ImmutableSortedStringSet of unique strings from an array
     */
    public static function __set_state(array $array): ImmutableSortedStringSet {}

    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     */
    public function add(mixed $value): bool {}

    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     * @implementation-alias Teds\ImmutableSortedStringSet::add
     */
    public function remove(mixed $value): bool {}

    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     */
    public function clear(): void {}

    /**
     * @throws \UnderflowException if there are no more elements
     */
    public function first(): string {}
    /**
     * @throws \UnderflowException if there are no more elements
     */
    public function last(): string {}

    /**
     * @psalm-return list<string>
     */
    public function toArray(): array {}
    /** @implementation-alias Teds\ImmutableSortedStringSet::toArray */
    public function values(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function get(int $offset): string {}

    /**
     * Returns the offset of a value that is === $value, or returns null.
     * (Uses binary search)
     */
    public function indexOf(string $value): ?int {}
    /**
     * Returns true if there exists a string === $value in this ImmutableSortedStringSet.
     * (Uses binary search)
     */
    public function contains(mixed $value): bool {}

    /**
     * @implementation-alias Teds\ImmutableSortedStringSet::toArray
     */
    public function jsonSerialize(): array {}
}
