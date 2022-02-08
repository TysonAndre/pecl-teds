<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * An ImmutableSequence is a container of a sequence of values (with keys 0, 1, ...count($vector) - 1)
 * that can't change in size and values can't change (but references in the values or properties of objects can change)
 *
 * This is backed by a memory-efficient representation
 * (raw array of values).
 */
final class ImmutableSequence implements \IteratorAggregate, \Countable, \JsonSerializable, \ArrayAccess
{
    public function __construct(iterable $iterator) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}
    /** Returns true if there are 0 elements in the ImmutableSequence. */
    public function isEmpty(): bool {}

    /**
     * @implementation-alias Teds\ImmutableSequence::toArray
     */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): ImmutableSequence {}

    public function toArray(): array {}
    // Strictly typed, unlike offsetGet
    public function get(int $offset): mixed {}
    // Must be mixed for compatibility with ArrayAccess
    public function offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): bool {}
    // Throws
    public function offsetSet(mixed $offset, mixed $value): void {}
    // Throws
    public function offsetUnset(mixed $offset): void {}

    public function indexOf(mixed $value): ?int {}
    public function contains(mixed $value): bool {}

    /**
     * Returns a new ImmutableSequence instance created from the return values of $callable($element)
     * being applied to each element of this sequence.
     *
     * (at)param null|callable(mixed):mixed $callback
     */
    public function map(callable $callback): ImmutableSequence {}
    /**
     * Returns the subset of elements of the ImmutableSequence satisfying the predicate.
     *
     * If the value returned by the callback is truthy
     * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
     * this is treated as satisfying the predicate.
     *
     * (at)param null|callable(mixed):bool $callback
     */
    public function filter(?callable $callback = null): ImmutableSequence {}
    /**
     * @implementation-alias Teds\ImmutableSequence::toArray
     */
    public function jsonSerialize(): array {}
}
