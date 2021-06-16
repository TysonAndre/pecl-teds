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

[`Teds\ImmutableKeyValueSequence` API](teds_immutablekeyvaluesequence.stub.php)

Currently, PHP does not provide a built-in way to store the state of an arbitrary iterable for reuse later (when the iterable has arbitrary keys, or when keys might be repeated). It would be useful to do so for many use cases, such as:

1. Creating a rewindable copy of a non-rewindable `Traversable` (e.g. a `Generator`) before passing that copy to a function that consumes an `iterable`/`Traversable`. (new ImmutableKeyValueSequence(my_generator()))
2. Generating an `IteratorAggregate` from a class still implementing `Iterator` (e.g. `SplObjectStorage`) so that code can independently iterate over the key-value sequences. (e.g. `foreach ($immutableKeyValueSequence as $k1 => $v1) { foreach ($immutableKeyValueSequence as $k2 => $v2) { /* process pairs */ } }`)
3. Providing helpers such as `iterable_flip(iterable $input)`, `iterable_take(iterable $input, int $limit)`, `iterable_chunk(iterable $input, int $chunk_size)` that act on iterables with arbitrary key/value sequences and have return values including iterables with arbitrary key/value sequences
4. Providing constant time access to both keys and values of arbitrary key-value sequences at any offset (for binary searching on keys and/or values, etc.)

Having this implemented as a native class would also allow it to be much more efficient than a userland solution (in terms of time to create, time to iterate over the result, and total memory usage).

## Related

- https://www.php.net/spl is built into php
- https://www.php.net/manual/en/book.ds.php
