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

### Teds\ImmutableIterable

[`Teds\ImmutableIterable` API](./teds_immutableiterable.stub.php)

Currently, PHP does not provide a built-in way to store the state of an arbitrary iterable for reuse later (when the iterable has arbitrary keys, or when keys might be repeated). There are use cases for this, such as:

1. Creating a rewindable copy of a non-rewindable `Traversable` (e.g. a `Generator`) before passing that copy to a function that consumes an `iterable`/`Traversable`. (new ImmutableIterable(my_generator()))
2. Generating an `IteratorAggregate` from a class still implementing `Iterator` (e.g. `SplObjectStorage`) so that code can independently iterate over the key-value sequences. (e.g. `foreach ($immutableImmutableIterable as $k1 => $v1) { foreach ($immutableImmutableIterable as $k2 => $v2) { /* process pairs */ } }`)
3. Providing helpers such as `iterable_flip(iterable $iterable)`, `iterable_take(iterable $iterable, int $limit)`, `iterable_chunk(iterable $iterable, int $chunk_size)` that act on iterables with arbitrary key/value sequences and have return values including iterables with arbitrary key/value sequences
4. Providing constant time access to both keys and values of arbitrary key-value sequences at any offset (for binary searching on keys and/or values, etc.)

Having this implemented as a native class allows it to be much more efficient than a userland solution (in terms of time to create, time to iterate over the result, and total memory usage, depending on the representation).

Objects within this data structure or references in arrays in this data structure can still be mutated.

Traversables are eagerly iterated over in the constructor.

### Teds\ImmutableSequence

[`Teds\ImmutableSequence` API](./teds_immutablesequence.stub.php)

Similar to SplFixedArray or Ds\Sequence, but immutable.
This stores a sequence of values with the keys 0, 1, 2....

### Teds\LowMemoryVector

[`Teds\LowMemoryVector` API](./teds_lowmemoryvector.stub.php)

This exposes the same API as a Vector, but with a more memory-efficient representation if the LowMemoryVector has only ever included a type that it could optimize.

Benefits:

- For collections of exclusively int, exclusively floats, or exclusively null/true/false, this uses less memory when serialized compared to vectors/arrays.
- For collections of exclusively int, exclusively floats, or exclusively null/true/false, this uses less memory when serialized compared to vectors/arrays.

  Note that adding other types will make this use as much memory as a Vector, e.g. adding any non-float (including int) to a collection of floats.
- Has faster checks for contains/indexOf (if values can have an optimized representation)
- Has faster garbage collection (if values can have an optimized representation due to int/float/bool/null not needing reference counting).
- Interchangeable with `Vector` or other collections without configuration - this will silently fall back to the default `mixed` representation if a more efficient representation is not supported.

Drawbacks:
- Slightly more overhead when types aren't specialized.
- Adding a different type to the collection will permanently make it used the less efficient `mixed` representation.

In 64-bit builds, the following types are supported, with the following amounts of memory (plus constant overhead to represent the LowMemoryVector itself, and extra capacity for growing the LowMemoryVector):

1. null/bool : 1 byte per value.

   (In serialize(), this is even less, adding 1 to 2 bits per value (2 bits if null is included)).
2. signed 8-bit int (-128..127): 1 byte per value. Adding a larger int to any of these n-bit types will convert them to the collection of that larger int type.
3. signed 16-bit int: 2 bytes per value.
4. signed 32-bit int: 4 bytes per value.
5. signed 64-bit int: 8 bytes per value. (64-bit php builds only)
6. signed PHP `float`:  8 bytes per value. (C `double`)
7. `mixed` or combinations of the above: 16 bytes per value. (Same as `Vector`)

In comparison, in 64-bit builds of PHP, PHP's arrays take at least 16 bytes per value in php 8.2, and at least 32 bytes per value before php 8.1, at the time of writing.

Example benchmarks: [benchmarks/benchmark_vector_bool.php](benchmarks/benchmark_vector_bool.php) and [benchmarks/benchmark_vector_unserialize.phpt](benchmarks/benchmark_vector_unserialize.phpt).

### Teds\IntVector

[`Teds\IntVector` API](./teds_intvector.stub.php)

Similar to `Teds\LowMemoryVector` but throws a TypeError on attempts to add non-integers.

### Teds\BitSet

[`Teds\BitSet` API](./teds_bitset.stub.php)

Similar to `Teds\LowMemoryVector`/`Teds\IntVector` but throws a TypeError on attempts to add non-booleans.
This can be used as a memory-efficient vector of booleans.

This uses only a single bit per value for large bit sets in memory and when serializing (around 128 times less memory than arrays, for large arrays of booleans)

### Teds\Vector

[`Teds\Vector` API](./teds_vector.stub.php)

Similar to SplFixedArray or Ds\Vector.
This stores a mutable sequence of values with the keys 0, 1, 2...
It can be appended to with `push()`, and elements can be removed from the end with `pop()`

This is implemented based on SplFixedArray/ImmutableSequence.
There are plans to add more methods.

### Teds\MutableIterable

[`Teds\MutableIterable` API](./teds_mutableiterable.stub.php)

Similar to `Teds\Vector` and `Teds\ImmutableIterable`.
This stores a mutable vector of keys and values with the keys 0, 1, 2...
It can be resized with `setSize()`.

### Teds\Deque

[`Teds\Deque` API](./teds_deque.stub.php)

Similar to SplDoublyLinkedList but backed by an array instead of a linked list.
Much more efficient in memory usage and random access than SplDoublyLinkedList.

(Also similar to `Ds\Deque`)

### Teds\StrictTreeMap

[`Teds\StrictTreeMap` API](./teds_stricthashmap.stub.php)

This is a map where entries for keys of any type can be inserted if `Teds\stable_compare !== 0`.

This uses a balanced [red-black tree](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) to ensure logarithmic time is needed for insertions/removals/lookups.

Iteration will stop if the current key of an iterator is removed.

This uses [`Teds\stable_compare`](#stable-comparison) internally.

The [`Teds\StrictTreeSet` API](./teds_stricttreeset.stub.php) implementation is similar, but does not associate values with keys. Also, `StrictTreeSet` does not implement ArrayAccess and uses different method names.

### Teds\StrictHashMap and Teds\StrictHashSet

[`Teds\StrictHashMap` API](./teds_stricthashmap.stub.php)

**This is a work in progress.** Iteration will not work as expected if the hash table is rehashed due to insertions/removals during iteration.

This is a map where entries for keys of any type can be inserted if they are `!==` to other keys.
This uses [`Teds\strict_hash`](#strict-hashing) internally.

The [`Teds\StrictHashSet` API](./teds_stricthashset.stub.php) implementation is similar, but does not associate values with keys and does not implement ArrayAccess and uses different method names.

NOTE: The floats `0.0` and [`-0.0` (negative zero)](https://en.wikipedia.org/wiki/Signed_zero) currently have distinct hashes and are treated as distinct entries.

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

### Common interfaces

[teds_interfaces.stub.php](./teds_interfaces.stub.php)

**NOTE: This is currently being revised, and new methods may be added to these interfaces in 0.x releases or new major releases.** More methods are currently being added.

These provide common interfaces for accessing the lists, sorted/hash sets, sorted/hash maps, sequences, and key value sequences that are provided by `Teds\`.

```
<?php
namespace Teds;

/**
 * Values is a common interface for an object with values that can be iterated over and counted,
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 */
interface Values extends \Traversable, \Countable {
    /** @return list<values> */
    public function values(): array {}

    /** Returns true if count() would be 0 */
    public function isEmpty(): bool {}

    /** Returns true if this contains a value identical to $value */
    public function contains(mixed $value): bool {}
}

/**
 * A sequence represents a list of values that can be iterated over and counted,
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 */
interface Collection extends Values, \ArrayAccess {
    /**
     * Returns a list or associative array,
     * typically attempting to cast keys to array key types (int/string) to insert elements of the collection into the array, or throwing.
     *
     * When this is impossible for the class in general,
     * this returns an array with representations of key/value entries of the Collection.
     */
    public function toArray(): array {}

    /**
     * Returns true if there exists a key identical to $key according to the semantics of the implementing collection.
     * Typically, this is close to the definition of `===`, but may be stricter or weaker, e.g. for NAN, negative zero, etc.
     */
    public function containsKey(mixed $value): bool {}
}

/**
 * but not addressed with ArrayAccess (e.g. Sets, Heaps, objects that yield values in an order in their iterators, etc.)
 *
 * Note: JsonSerializable was not included here because
 * 1. Causes issues or inefficiencies with some ways of implementing data structures internally.
 * 2. Forces the result to be represented as []
 * 3. Forces polyfills to implement JsonSerializable as well, even when it would be less efficient
 *
 * NOTE: List is a reserved keyword in php and cannot be used as an identifier, e.g. `list($x) = [1]`.
 */
interface Sequence extends Collection {
    public function get(int $key): mixed {}
    public function set(int $offset, mixed $value): void {}

    public function push(mixed ...$values): void {}
    public function pop(): mixed {}
}
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

/**
 * Returns a list of unique values in order of occurrence,
 * using a hash table with `Teds\strict_hash` to deduplicate values.
 */
function unique_values(iterable $iterable): array {
	// Without Teds installed, this takes quadratic time instead of linear time.
	$result = [];
	foreach ($iterable as $value) {
		if (!in_array($value, $result, true)) {
			$result[] = $value;
		}
	}
	return $result;
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

## Binary search

`Teds\binary_search(array $values, mixed $target, callable $comparer = null, bool $useKey=false)`
can be used to [binary search](https://en.wikipedia.org/wiki/Binary_search_algorithm) on arrays that are sorted by key (ksort, uksort) or value (sort, usort, uasort).
(even if keys were unset).

This will have unpredictable results if the array is out of order. See [`Teds\stable_sort`](#stable-comparison) for ways to sort even arbitrary values in a stable order.

This is faster for very large sorted arrays. See [benchmarks](benchmarks/).

This returns the key and value of the first entry `<=` $needle according to the comparer, and whether an entry comparing equal was found.
By default, php's default comparison behavior (`<=>`) is used.

```
php > $values = [1 => 100, 3 => 200, 4 => 1000];
php > echo json_encode(Teds\binary_search($values, 1));
{"found":false,"key":null,"value":null}
php > echo json_encode(Teds\binary_search($values, 100));
{"found":true,"key":1,"value":100}
php > echo json_encode(Teds\binary_search($values, 201));
{"found":false,"key":3,"value":200}
php > echo json_encode(Teds\binary_search($values, 99));
{"found":false,"key":null,"value":null}
php > echo json_encode(Teds\binary_search($values, 1, useKey: true));
{"found":true,"key":1,"value":100}
```


## Motivation

This contains functionality and data structures that may be proposed for inclusion into PHP itself (under a different namespace) at a future date, reimplemented using [SPL's source code](https://github.com/php/php-src/tree/master/ext/spl) as a starting point.

Providing this as a PECL first makes this functionality easier to validate for correctness, and make it more practical to change APIs before proposing including them in PHP if needed.

## License

See [COPYING](./COPYING)

## Changelog

See [package.xml](./package.xml)

## Related Projects

- https://www.php.net/spl is built into php
- https://www.php.net/manual/en/book.ds.php
