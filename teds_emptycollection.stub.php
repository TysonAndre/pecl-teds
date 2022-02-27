<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer for enums.

#if PHP_VERSION_ID >= 80100
namespace Teds;

/**
 * An EmptySequence is an immutable sequence of length 0.
 * NOTE: This is only available in php 8.1+
 */
enum EmptySequence implements \Iterator, Sequence, \JsonSerializable
{
    case INSTANCE;

    /** @return 0 */
    public function count(): int {}
    /** @return true */
    public function isEmpty(): bool {}

    public function toArray(): array {}
    /**
     * @override
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function values(): array {}
    /**
     * @throws \OutOfBoundsException unconditionally
     */
    public function get(int $offset): never {}
    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     */
    public function set(int $offset, mixed $value): never {}
    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     */
    public function insert(int $offset, mixed ...$values): never {}
    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     */
    public function push(mixed ...$values): never {}
    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     */
    public function pop(): never {}
    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     * @implementation-alias Teds\EmptySequence::push
     */
    public function unshift(mixed ...$values): never {}
    /**
     * @throws \Teds\UnsupportedOperationException unconditionally
     * @implementation-alias Teds\EmptySequence::pop
     */
    public function shift(): never {}

    /**
     * @throws \UnderflowException unconditionally
     */
    public function first(): never {}
    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function last(): never {}
    /**
     * Does nothing, does not throw. This is already empty.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function clear(): void {}
    // Must be mixed for compatibility with ArrayAccess
    /**
     * @throws \OutOfBoundsException unconditionally
     * @return never
     */
    public function offsetGet(mixed $offset): mixed {}
    /**
     * @return false
     */
    public function offsetExists(mixed $offset): bool {}
    /**
     * @return false
     * @implementation-alias Teds\EmptySequence::offsetExists
     */
    public function containsKey(mixed $offset): bool {}
    /**
     * @throws \OutOfBoundsException unconditionally
     * @return never
     */
    public function offsetSet(mixed $offset, mixed $value): void {}
    /**
     * @throws \OutOfBoundsException unconditionally
     * @return never
     */
    public function offsetUnset(mixed $offset): void {}

    /**
     * @return null
     */
    public function indexOf(mixed $value): ?int {}
    /**
     * @return false
     * @implementation-alias Teds\EmptySequence::offsetExists
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns the EmptySequence instance.
     */
    public function map(callable $callback): EmptySequence {}
    /**
     * Returns the EmptySequence instance.
     */
    public function filter(?callable $callback = null): EmptySequence {}
    /**
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function jsonSerialize(): array {}

    /** @return false */
    public function valid(): bool { }

    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function current(): never { }
    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function key(): never { }
    /**
     * Does nothing.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function next(): void { }
    /**
     * Does nothing.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function rewind(): void { }
}

/**
 * Immutable empty map.
 */
enum EmptyMap implements \Iterator, Map, \JsonSerializable
{
    case INSTANCE;

    /**
     * Returns the number of elements in the EmptyMap.
     * @implementation-alias Teds\EmptySequence::count
     */
    public function count(): int {}
    /**
     * Returns true if there are 0 elements in the EmptyMap.
     * @implementation-alias Teds\EmptySequence::isEmpty
     */
    public function isEmpty(): bool {}
    /**
     * Removes all elements from the EmptyMap. (no-op)
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function clear(): void {}

    /**
     * Returns the empty array
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function toArray(): array {}
    /**
     * Returns the empty array
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function values(): array {}

    /**
     * Returns the empty array
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function keys(): array {}

    /**
     * Returns the value for the given key.
     * @throws \OutOfBoundsException if $key is not found and $default was not provided.
     */
    public function offsetGet(mixed $key): never {}
    /**
     * Returns true if the value for the key exists and is not null
     * @return false
     * @implementation-alias Teds\EmptySequence::contains
     */
    public function offsetExists(mixed $key): bool {}
    /**
     * Sets the value at the given key.
     * @implementation-alias Teds\EmptySequence::offsetSet
     */
    public function offsetSet(mixed $key, mixed $value): never {}
    /**
     * No-op for empty map.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function offsetUnset(mixed $key): void {}

    /**
     * Returns true if there exists a value === $value in this EmptyMap.
     * @implementation-alias Teds\EmptySequence::contains
     * @return false
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if there exists a key === $key in this EmptyMap.
     * @implementation-alias Teds\EmptySequence::contains
     * @return false
     */
    public function containsKey(mixed $key): bool {}

    /**
     * Returns the value at $key.
     * @throws \OutOfBoundsException if $default was not provided.
     */
    public function get(mixed $key, mixed $default = null): mixed {}

    /**
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function jsonSerialize(): array {}

    /**
     * @return false
     * @implementation-alias Teds\EmptySequence::valid
     */
    public function valid(): bool { }

    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function current(): never { }
    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function key(): never { }
    /**
     * Does nothing.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function next(): void { }
    /**
     * Does nothing.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function rewind(): void { }
}

/**
 * Immutable empty set.
 */
enum EmptySet implements \Iterator, Set, \JsonSerializable
{
    case INSTANCE;

    /**
     * Returns the number of elements in the EmptySet
     * @implementation-alias Teds\EmptySequence::count
     */
    public function count(): int {}
    /**
     * Returns true if there are 0 elements in the EmptySet.
     * @implementation-alias Teds\EmptySequence::isEmpty
     */
    public function isEmpty(): bool {}
    /**
     * Removes all elements from the EmptySet. (no-op)
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function clear(): void {}

    /**
     * Returns the empty array
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function toArray(): array {}
    /**
     * Returns the empty array
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function values(): array {}

    /**
     * @return false
     * @implementation-alias Teds\EmptySequence::offsetExists
     */
    public function contains(mixed $value): bool {}

    /**
     * Returns true if $value was added to this EmptySet and was not previously in this EmptySet.
     * @implementation-alias Teds\EmptySequence::offsetUnset
     */
    public function add(mixed $value): never {}

    /**
     * Returns true if $value was found in this EmptySet before being removed from this EmptySet.
     * @return false
     * @implementation-alias Teds\EmptySequence::contains
     */
    public function remove(mixed $value): bool {}

    /**
     * Returns [v1, v2, ...]
     * @implementation-alias Teds\EmptySequence::toArray
     */
    public function jsonSerialize(): array {}

    /**
     * @return false
     * @implementation-alias Teds\EmptySequence::valid
     */
    public function valid(): bool { }
    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function current(): never { }
    /**
     * @implementation-alias Teds\EmptySequence::first
     */
    public function key(): never { }
    /**
     * Does nothing.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function next(): void { }
    /**
     * Does nothing.
     * @implementation-alias Teds\StrictMinHeap::rewind
     */
    public function rewind(): void { }
}
#endif
