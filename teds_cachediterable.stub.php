<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/**
 * A cached iterable containing keys and values, where keys are repeatable and can be any type.
 *
 * When passed a Traversable, this will only evaluate elements when they are needed.
 */
final class CachedIterable implements \IteratorAggregate, Collection, \JsonSerializable
{
    public function __construct(iterable $iterator) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}
    /** Returns true if count() == 0. */
    public function isEmpty(): bool {}
    /**
     * @implementation-alias Teds\CachedIterable::__set_state
     */
    public static function fromPairs(array $pairs): CachedIterable {}
    public function toPairs(): array {}
    /**
     * Returns an array by attempting to cast keys to array key types (int/string)
     * to insert elements of the collection into the array, or throwing/emitting notices.
     */
    public function toArray(): array {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): CachedIterable {}

    public function keys(): array {}
    public function values(): array {}
    public function keyAt(int $offset): mixed {}
    public function valueAt(int $offset): mixed {}
    /**
     * @throws \Teds\UnsupportedOperationException
     * @return never
     */
    public function clear(): void {}

    public function indexOfKey(mixed $key): ?int {}
    public function indexOfValue(mixed $value): ?int {}
    public function containsKey(mixed $key): bool {}
    /**
     * @implementation-alias Teds\CachedIterable::contains
     */
    public function containsValue(mixed $value): bool {}
    public function contains(mixed $value): bool {}

    /** @implementation-alias Teds\CachedIterable::toPairs */
    public function jsonSerialize(): array {}
}
