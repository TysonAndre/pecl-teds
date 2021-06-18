# Teds

## Introduction

Teds is a another collection of data structures, maintained by Tyson.
(\[Tyson's|Tentative\] Extra Data Structures)

## Installation

This extension requires php 8.0 or newer.

```
phpize
./configure
make install
```

On Windows, see https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2 instead

## Functionality

### Teds\ImmutableKeyValueSequence

[`Teds\ImmutableKeyValueSequence` API](./teds_immutablekeyvaluesequence.stub.php)

Currently, PHP does not provide a built-in way to store the state of an arbitrary iterable for reuse later (when the iterable has arbitrary keys, or when keys might be repeated). It would be useful to do so for many use cases, such as:

1. Creating a rewindable copy of a non-rewindable `Traversable` (e.g. a `Generator`) before passing that copy to a function that consumes an `iterable`/`Traversable`. (new ImmutableKeyValueSequence(my_generator()))
2. Generating an `IteratorAggregate` from a class still implementing `Iterator` (e.g. `SplObjectStorage`) so that code can independently iterate over the key-value sequences. (e.g. `foreach ($immutableKeyValueSequence as $k1 => $v1) { foreach ($immutableKeyValueSequence as $k2 => $v2) { /* process pairs */ } }`)
3. Providing helpers such as `iterable_flip(iterable $input)`, `iterable_take(iterable $input, int $limit)`, `iterable_chunk(iterable $input, int $chunk_size)` that act on iterables with arbitrary key/value sequences and have return values including iterables with arbitrary key/value sequences
4. Providing constant time access to both keys and values of arbitrary key-value sequences at any offset (for binary searching on keys and/or values, etc.)

Having this implemented as a native class would also allow it to be much more efficient than a userland solution (in terms of time to create, time to iterate over the result, and total memory usage).

### iterable functions

This PECL contains a library of native implementations of various functions acting on iterables.
See [`teds.stub.php`](./teds.stub.php) for function signatures.

**The behavior is equivalent to the following polyfill**
(similarly to array_filter, the native implementation is likely faster than the polyfill with no callback, and slower with a callback)

```php
namespace PHP\iterable;

/**
 * Determines whether any element of the iterable satisfies the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * @param iterable $input
 * @param null|callable(mixed):mixed $callback
 */
function any(iterable $input, ?callable $callback = null): bool {
    foreach ($input as $v) {
        if ($callback !== null ? $callback($v) : $v) {
            return true;
        }
    }
    return false;
}
/**
 * Determines whether all elements of the iterable satisfy the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * @param iterable $input
 * @param null|callable(mixed):mixed $callback
 */
function all(iterable $input, ?callable $callback = null): bool {
    foreach ($input as $v) {
        if (!($callback !== null ? $callback($v) : $v)) {
            return false;
        }
    }
    return true;
}
```

## Related

- https://www.php.net/spl is built into php
- https://www.php.net/manual/en/book.ds.php
