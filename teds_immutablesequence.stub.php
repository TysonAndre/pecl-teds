<?php

/** @generate-class-entries */

namespace Teds {

final class ImmutableSequence implements IteratorAggregate, Countable, JsonSerializable, ArrayAccess
{
    public function __construct(iterable $iterator) {}
    public function getIterator(): \InternalIterator {}
    public function count(): int {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): ImmutableSequence {}

    public function toArray(): array {}
    // Strictly typed, unlike offsetGet
    public function valueAt(int $offset): mixed {}
    // Must be mixed for compatibility with ArrayAccess
    public function offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): mixed {}
    // Throws
    public function offsetSet(mixed $offset, mixed $value): mixed {}
    // Throws
    public function offsetUnset(mixed $offset): mixed {}

    public function jsonSerialize(): array {}
}

}
