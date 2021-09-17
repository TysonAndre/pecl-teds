<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stubs.php from php 8.1 or newer.

namespace Teds;

/**
 * A mutable vector of keys and values, where keys are repeatable and can be any type.
 */
final class KeyValueVector implements \IteratorAggregate, \Countable, \JsonSerializable
{
    public function __construct(iterable $iterator = []) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}
    public function capacity(): int {}
    public function clear(): void {}
    public function setSize(int $size): void {}
    public static function fromPairs(iterable $pairs): KeyValueVector {}
    public function toPairs(): array {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): KeyValueVector {}

    public function push(mixed $key, mixed $value): void {}
    public function pop(): array {}

    public function keys(): array {}
    public function values(): array {}
    public function keyAt(int $offset): mixed {}
    public function valueAt(int $offset): mixed {}
    public function setKeyAt(int $offset, mixed $key): void {}
    public function setValueAt(int $offset, mixed $value): void {}

    public function indexOfKey(mixed $key): ?int {}
    public function indexOfValue(mixed $value): ?int {}
    public function containsKey(mixed $key): bool {}
    public function containsValue(mixed $value): bool {}

    public function shrinkToFit(): void {}

    public function jsonSerialize(): array {}
}
