# Teds

## Introduction

[![Build Status](https://github.com/TysonAndre/pecl-teds/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/TysonAndre/pecl-teds/actions/workflows/main.yml?query=branch%3Amain)
[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/j5s46tetcg4101g8?svg=true)](https://ci.appveyor.com/project/TysonAndre/pecl-teds)

Teds is a another collection of data structures.
(Tentative Extra Data Structures)

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

### Teds\KeyValueVector

[`Teds\KeyValueVector` API](./teds_keyvaluevector.stub.php)

Similar to `Teds\Vector` and `Teds\ImmutableKeyValueSequence`.
This stores a mutable vector of keys and values with the keys 0, 1, 2...
It can be resized with `setSize()`.

### Teds\Deque

[`Teds\Deque` API](./teds_deque.stub.php)

Similar to SplDoublyLinkedList but backed by an array instead of a linked list.
Much more efficient in memory usage and random access than SplDoublyLinkedList.

(Also similar to `Ds\Deque`)

### Teds\SortedStrictMap

[`Teds\SortedStrictMap` API](./teds_strictmap.stub.php)

**This is a work in progress.** Iteration will not work as expected if elements are removed during iteration, and this is backed by a raw array (using insertion sort) instead of a balanced binary tree, so insertion/removals are inefficient for large maps.

This is a map where entries for keys of any type can be inserted if `Teds\stable_compare !== 0`.
This uses [`Teds\stable_compare`](#stable-comparison) internally.

The [`Teds\SortedStrictSet` API](./teds_sortedstrictset.stub.php) implementation is similar, but does not associate values with keys and does not implement ArrayAccess and uses different method names.

### Teds\StrictMap and Teds\StrictSet

[`Teds\StrictMap` API](./teds_strictmap.stub.php)

**This is a work in progress.** Iteration will not work as expected if elements are removed during iteration, and hash lookups haven't been implemented yet, so this is inefficient for large maps (scans the entire map).

This is a map where entries for keys of any type can be inserted if they are `!==` to other keys.
This uses [`Teds\strict_hash`](#strict-hashing) internally.

The [`Teds\StrictSet` API](./teds_strictset.stub.php) implementation is similar, but does not associate values with keys and does not implement ArrayAccess and uses different method names.

### Teds\StableMinHeap and Teds\StableMaxHeap

[`Teds\Stable*Heap` API/polyfill](./teds_stableheap.stub.php)

This uses `Teds\stable_compare` instead of PHP's unstable default comparisons.
Sorting logic can be customized by inserting `[$priority, $value]` instead of `$value`.
(Or by subclassing `SplMinHeap`/`SplMaxHeap` and overriding `compare` manually).

```
php > $x = new SplMinHeap();
php > foreach (['19', '9', '2b', '2'] as $v) { $x->insert($v); }
php > foreach ($x as $value) { echo "$value,"; } echo "\n"; // unpredictable order
2,19,2b,9,

php > $x = new Teds\StableMinHeap();
php > foreach (['19', '9', '2b', '2'] as $v) { $x->insert($v); }
php > foreach ($x as $value) { echo "$value,"; } echo "\n"; // lexicographically sorted
19,2,2b,9,
```

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

/**
 * Similar to in_array($value, $array, true) but also works on Traversables.
 */
function includes_value(iterable $iterable, mixed $value): bool {
	foreach ($iterable as $other) {
		if ($other === $value) {
			return true;
		}
	}
	return false;
}
```

## Stable comparison

`Teds\stable_compare` is a function that can be used to compare arbitrary values in a stable order.

This exists because php's `<` operator is not stable. `'10' < '0a' < '1b' < '9' < '10'`.
`Teds\stable_compare` fixes that by strictly ordering:

- `null < false < true < int,float < string < array < object < resource`.
- objects are compared by class name with strcmp, then by spl_object_id.
- resources are compared by id.
- arrays are compared recursively. Smaller arrays are less than larger arrays.
- int/float are compared numerically. If an int is equal to a float, then the int is first.
- strings are compared with strcmp.

## Strict hashing

`Teds\strict_hash` provides a hash based on value identity.

- Objects are hashed based on `spl_object id`.
  Different objects will have different hashes for the lifetime of the hash.
- Resources are hashed based on `get_resource_id`.
- Strings are hashed
- References are dereferenced and hashed the same way as the value.
- Integers are returned
- Floats are hashed in a possibly platform-specific way.
- Arrays are hashed recursively. If $a1 === $a2 then they will have the same hash.

This may vary based on php release, OS, CPU architecture, or Teds release
and should not be saved/loaded outside of a given php process.
(and `spl_object_id`/`get_resource_id` are unpredictable)

## Motivation

This contains functionality and data structures that may be proposed for inclusion into PHP itself (under a different namespace) at a future date, reimplemented using [SPL's source code](https://github.com/php/php-src/tree/master/ext/spl) as a starting point.

Providing this as a PECL first would make this functionality easier to validate for correctness, and make it more practical to change APIs before proposing including them in PHP if needed.

## License

See [COPYING](./COPYING)

## Related Projects

- https://www.php.net/spl is built into php
- https://www.php.net/manual/en/book.ds.php
