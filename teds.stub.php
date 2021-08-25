<?php
/** @generate-class-entries */
// Stub generation requires build/gen_stubs.php from php 8.1 or newer.

// NOTE: Due to a limitation of gen_stubs.php (at)param is used instead

namespace Teds;

/**
 * Determines whether any element of the iterable satisfies the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * (at)param null|callable(mixed):mixed $callback
 */
function any(iterable $iterable, ?callable $callback = null): bool {}

/**
 * Determines whether all elements of the iterable satisfy the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * (at)param null|callable(mixed):mixed $callback
 */
function all(iterable $iterable, ?callable $callback = null): bool {}

/**
 * Determines whether no element of the iterable satisfies the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * Equivalent to `!any($iterable, $callback)`
 *
 * (at)param null|callable(mixed):mixed $callback
 */
function none(iterable $iterable, ?callable $callback = null): bool {}

/**
 * Returns true if this iterable includes a value identical to $value (===).
 * Similar to in_array($value, $iterable, true) but also works on Traversables
 */
function includes_value(iterable $iterable, mixed $value): bool {}

/**
 * Returns the result of repeatedly applying $initial = $callback($initial, $value) for each value in $iterable
 * (at)param callable(mixed $initial, mixed $value):mixed $callback
 */
function fold(iterable $iterable, callable $callback, mixed $initial): mixed {}

/**
 * Returns the first value for which $callback($value) is truthy.
 */
function find(iterable $iterable, callable $callback, mixed $default = null): mixed {}
