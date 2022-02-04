<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

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
     * @implementation-alias Teds\ImmutableSequence::toArray
     */
    public function jsonSerialize(): array {}
}
