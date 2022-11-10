<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * A mutable vector of keys and values, where keys are repeatable and can be any type.
 * @alias Teds\KeyValueSequence
 * (This alias will be removed in a future major Teds release. Do not use it.)
 */
final class MutableIterable implements \IteratorAggregate, Collection, \JsonSerializable
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
    public static function fromPairs(iterable $pairs): MutableIterable {}
    public function toPairs(): array {}

    /** Returns [key1, value1, key2, value2, ...] */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): MutableIterable {}

    public function push(mixed $key, mixed $value): void {}
    public function pop(): array {}

    /** @psalm-return list<mixed> */
    public function keys(): array {}
    /** @psalm-return list<mixed> */
    public function values(): array {}
    /**
     * Returns an array created by inserting values for the corresponding keys, like iterator_to_array()
     */
    public function toArray(): array {}
    public function keyAt(int $offset): mixed {}
    public function valueAt(int $offset): mixed {}
    public function setKeyAt(int $offset, mixed $key): void {}
    public function setValueAt(int $offset, mixed $value): void {}

    public function indexOfKey(mixed $key): ?int {}
    public function indexOfValue(mixed $value): ?int {}
    public function containsKey(mixed $key): bool {}
    /**
     * @implementation-alias Teds\MutableIterable::contains
     */
    public function containsValue(mixed $value): bool {}
    public function contains(mixed $value): bool {}

    public function shrinkToFit(): void {}

    /** @implementation-alias Teds\MutableIterable::toPairs */
    public function jsonSerialize(): array {}
}
