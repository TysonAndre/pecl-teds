<?php

/** @generate-class-entries */
// Stub generation requires build/gen_stub.php from php 8.1 or newer.

namespace Teds;

/** @generate-class-entries */

final class StableMinHeap implements \Iterator, Values
{
    public function __construct(iterable $values = []) {}

    public function add(mixed $value): void {}

    public function top(): mixed {}

    public function extract(): mixed {}

    public function count(): int {}

    public function isEmpty(): bool {}

    public function rewind(): void {}

    /**
     * @implementation-alias Teds\StableMinHeap::top
     */
    public function current(): mixed {}

    /**
     * @implementation-alias Teds\StableMinHeap::top
     */
    public function key(): mixed {}

    public function next(): void {}

    public function valid(): bool {}

    public function clear(): void {}

    public static function __set_state(array $state): StableMinHeap { }

    /** @return list<mixed> */
    public function values(): array {}

    public function contains(mixed $value): bool {}

    /** @implementation-alias Teds\StableMinHeap::values */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}

final class StableMaxHeap implements \Iterator, Values
{
    public function __construct(iterable $values = []) {}

    public function add(mixed $value): void {}

    /** @implementation-alias Teds\StableMinHeap::top */
    public function top(): mixed {}

    public function extract(): mixed {}

    /** @implementation-alias Teds\StableMinHeap::count */
    public function count(): int {}

    /** @implementation-alias Teds\StableMinHeap::isEmpty */
    public function isEmpty(): bool {}

    /** @implementation-alias Teds\StableMinHeap::rewind */
    public function rewind(): void {}

    /** @implementation-alias Teds\StableMinHeap::top */
    public function current(): mixed {}

    /** @implementation-alias Teds\StableMinHeap::top */
    public function key(): mixed {}

    public function next(): void {}

    /** @implementation-alias Teds\StableMinHeap::valid */
    public function valid(): bool {}

    /** @implementation-alias Teds\StableMinHeap::clear */
    public function clear(): void {}

    public static function __set_state(array $state): StableMaxHeap { }

    /** @implementation-alias Teds\StableMinHeap::values */
    public function values(): array {}

    /** @implementation-alias Teds\StableMinHeap::contains */
    public function contains(mixed $value): bool {}

    /** @implementation-alias Teds\StableMinHeap::values */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
}
