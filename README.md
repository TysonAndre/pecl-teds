# Teds

## Introduction

[![Build Status](https://github.com/TysonAndre/pecl-teds/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/TysonAndre/pecl-teds/actions/workflows/main.yml?query=branch%3Amain)
[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/j5s46tetcg4101g8?svg=true)](https://ci.appveyor.com/project/TysonAndre/pecl-teds)

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
3. Providing helpers such as `iterable_flip(iterable $iterable)`, `iterable_take(iterable $iterable, int $limit)`, `iterable_chunk(iterable $iterable, int $chunk_size)` that act on iterables with arbitrary key/value sequences and have return values including iterables with arbitrary key/value sequences
4. Providing constant time access to both keys and values of arbitrary key-value sequences at any offset (for binary searching on keys and/or values, etc.)

Having this implemented as a native class would also allow it to be much more efficient than a userland solution (in terms of time to create, time to iterate over the result, and total memory usage).

Objects within this data structure or references in arrays in this data structure can still be mutated.

### Teds\ImmutableSequence

[`Teds\ImmutableKeyValueSequence` API](./teds_immutablekeyvaluesequence.stub.php)

Similar to SplFixedArray or Ds\Sequence, but immutable.
This stores a sequence of values with the keys 0, 1, 2....

### Teds\Vector

[`Teds\Vector` API](./teds_vector.stub.php)

Similar to SplFixedArray or Ds\Vector.
This stores a mutable sequence of values with the keys 0, 1, 2...
It can be appended to with `push()`, and elements can be removed from the end with `pop()`

This is implemented based on SplFixedArray/ImmutableSequence.
There are plans to add more methods.

### iterable functions

This PECL contains a library of native implementations of various functions acting on iterables.
See [`teds.stub.php`](./teds.stub.php) for function signatures.

**The behavior is equivalent to the following polyfill**
(similarly to array_filter, the native implementation is likely faster than the polyfill with no callback, and slower with a callback)

```php
namespace Teds;

/**
 * Determines whether any element of the iterable satisfies the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * @param iterable $iterable
 * @param null|callable(mixed):mixed $callback
 */
function any(iterable $iterable, ?callable $callback = null): bool {
    foreach ($iterable as $v) {
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
 * @param iterable $iterable
 * @param null|callable(mixed):mixed $callback
 */
function all(iterable $iterable, ?callable $callback = null): bool {
    foreach ($iterable as $v) {
        if (!($callback !== null ? $callback($v) : $v)) {
            return false;
        }
    }
    return true;
}

/**
 * Determines whether no element of the iterable satisfies the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * @param iterable $iterable
 * @param null|callable(mixed):mixed $callback
 */
function none(iterable $iterable, ?callable $callback = null): bool {
	return !any($iterable, $callback);
}

// Analogous to array_reduce but with mandatory default
function fold(iterable $iterable, callable $callback, mixed $default): bool {
	foreach ($iterable as $value) {
		$default = $callback($default, $value);
	}
	return $default;
}

/**
 * Returns the first value for which $callback($value) is truthy.
 */
function find(iterable $iterable, callable $callback, mixed $default = null): bool {
	foreach ($iterable as $value) {
		if ($callback($value)) {
			return $value;
		}
	}
	return $default;
}

```

## Related

- https://www.php.net/spl is built into php
- https://www.php.net/manual/en/book.ds.php
