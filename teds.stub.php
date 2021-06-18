<?php
/** @generate-class-entries */

// NOTE: Due to a limitation of gen_stubs.php (at)param is used instead

namespace Teds {
/**
 * Determines whether any element of the iterable satisfies the predicate.
 *
 * If the value returned by the callback is truthy
 * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
 * this is treated as satisfying the predicate.
 *
 * @param iterable $iterable
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
 * @param iterable $iterable
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
 * @param iterable $iterable
 * (at)param null|callable(mixed):mixed $callback
 */
function none(iterable $iterable, ?callable $callback = null): bool {}

// Analogous to array_reduce
function reduce(iterable $iterable, callable $callback, mixed $initial = null): mixed {}

/**
 * Returns the first value for which $callback($value) is truthy.
 */
function find(iterable $iterable, callable $callback, mixed $default = null): mixed {}

}
