<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * A mutable vector of keys and values, where keys are repeatable and can be any type.
 */
final class KeyValueVector implements \IteratorAggregate, \Countable, \JsonSerializable
{
    public function __construct(iterable $iterator = []) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}
    /** Returns true if count() == 0. */
    public function isEmpty(): bool {}
    public function capacity(): int {}
    public function clear(): void {}
    /* TODO setSize($size, $value=null, $key=null) */
    public function setSize(int $size): void {}
    public static function fromPairs(iterable $pairs): KeyValueVector {}
    public function toPairs(): array {}

    /** Returns [key1, value1, key2, value2, ...] */
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

    /** @implementation-alias Teds\KeyValueVector::toPairs */
    public function jsonSerialize(): array {}
}
