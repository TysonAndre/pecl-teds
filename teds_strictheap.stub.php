<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/** @generate-class-entries */

/**
 * Represents a min heap using `Teds\stable_Compare`.
 *
 * @alias Teds\StableMinHeap
 * (This alias will be removed in a future major Teds release. Do not use it)
 */
final class StrictMinHeap implements \Iterator, Collection
{
    public function __construct(iterable $values = []) {}

    public function add(mixed $value): void {}

    public function top(): mixed {}

    public function extract(): mixed {}

    public function count(): int {}

    public function isEmpty(): bool {}

    public function rewind(): void {}

    /**
     * @implementation-alias Teds\StrictMinHeap::top
     */
    public function current(): mixed {}

    /**
     * @implementation-alias Teds\StrictMinHeap::top
     */
    public function key(): mixed {}

    public function next(): void {}

    public function valid(): bool {}

    public function clear(): void {}

    public static function __set_state(array $state): StrictMinHeap { }

    /** @psalm-return list<mixed> */
    public function values(): array {}

    /**
     * @psalm-return array mapping entries to themselves or throwing.
     * @see self::values
     */
    public function toArray(): array {}

    public function contains(mixed $value): bool {}

    /** @implementation-alias Teds\StrictMinHeap::values */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

/**
 * Represents a max heap using `Teds\stable_Compare`.
 *
 * @alias Teds\StableMaxHeap
 * (This alias will be removed in a future major Teds release. Do not use it)
 */
final class StrictMaxHeap implements \Iterator, Collection
{
    public function __construct(iterable $values = []) {}

    public function add(mixed $value): void {}

    /** @implementation-alias Teds\StrictMinHeap::top */
    public function top(): mixed {}

    public function extract(): mixed {}

    /** @implementation-alias Teds\StrictMinHeap::count */
    public function count(): int {}

    /** @implementation-alias Teds\StrictMinHeap::isEmpty */
    public function isEmpty(): bool {}

    /** @implementation-alias Teds\StrictMinHeap::rewind */
    public function rewind(): void {}

    /** @implementation-alias Teds\StrictMinHeap::top */
    public function current(): mixed {}

    /** @implementation-alias Teds\StrictMinHeap::top */
    public function key(): mixed {}

    public function next(): void {}

    /** @implementation-alias Teds\StrictMinHeap::valid */
    public function valid(): bool {}

    /** @implementation-alias Teds\StrictMinHeap::clear */
    public function clear(): void {}

    public static function __set_state(array $state): StrictMaxHeap { }

    /** @implementation-alias Teds\StrictMinHeap::values */
    public function values(): array {}

    /**
     * @psalm-return array mapping entries to themselves or throwing.
     * @see self::values
     * @implementation-alias Teds\StrictMinHeap::toArray
     */
    public function toArray(): array {}

    /**
     * @implementation-alias Teds\StrictMinHeap::toArray
     * @psalm-return array mapping entries to themselves or throwing.
     * @see self::values
     */
    /** @implementation-alias Teds\StrictMinHeap::contains */
    public function contains(mixed $value): bool {}

    /** @implementation-alias Teds\StrictMinHeap::values */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}
