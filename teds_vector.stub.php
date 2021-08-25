<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stubs.php from php 8.1 or newer.

namespace Teds;

final class Vector implements \IteratorAggregate, \Countable, \JsonSerializable, \ArrayAccess
{
    public function __construct(iterable $iterator = []) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}
    public function capacity(): int {}
    public function clear(): void {}
    public function setSize(int $size): void {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): Vector {}

    public function push(mixed $value): void {}
    public function pop(): mixed {}

    public function toArray(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function valueAt(int $offset): mixed {}
    public function setValueAt(int $offset, mixed $value): void {}
    // TODO public function setValueAt(int $offset, mixed $value): mixed {}
    // Must be mixed for compatibility with ArrayAccess
    public function offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): bool {}
    public function offsetSet(mixed $offset, mixed $value): void {}
    // Throws
    public function offsetUnset(mixed $offset): void {}

    public function indexOf(mixed $value): int|false {}
    public function contains(mixed $value): bool {}

    public function shrinkToFit(): void {}

    public function jsonSerialize(): array {}
}
