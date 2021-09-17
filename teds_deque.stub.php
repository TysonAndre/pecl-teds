<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stubs.php from php 8.1 or newer.

namespace Teds;

/**
 * A double-ended queue backed by an array. This has lower memory usage than SplDoublyLinkedList or its subclasses.
 */
final class Deque implements \IteratorAggregate, \Countable, \JsonSerializable, \ArrayAccess
{
    /* Construct the deque from the values of the Traversable/array, ignoring keys */
    public function __construct(iterable $iterator = []) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}
    public function capacity(): int {}
    public function clear(): void {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): Deque {}

    /**
     * Add an element to the end of the deque.
     */
    public function pushBack(mixed $value): void {}
    /**
     * Add an element to the start of the deque.
     */
    public function pushFront(mixed $value): void {}
    /**
     * Remove an element from the back of the deque and return its value.
     */
    public function popBack(): mixed {}
    /**
     * Remove an element from the front of the deque and return its value.
     */
    public function popFront(): mixed {}

    public function toArray(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function valueAt(int $offset): mixed {}
    public function setValueAt(int $offset, mixed $value): void {}
    // Must be mixed for compatibility with ArrayAccess
    public function offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): bool {}
    public function offsetSet(mixed $offset, mixed $value): void {}
    // Throws
    public function offsetUnset(mixed $offset): void {}

    public function indexOf(mixed $value): ?int {}
    public function contains(mixed $value): bool {}

    public function jsonSerialize(): array {}
}
