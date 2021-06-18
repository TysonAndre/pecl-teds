--TEST--
Test fold() function on Traversable
--FILE--
<?php

use function Teds\fold;

/*
    Prototype: mixed Teds\fold(array $array, callable($carry, $item): mixed $callback);
    Description: Iterate over iterable and fold
*/

function dump_fold(...$args) {
    try {
        var_dump(fold(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

function generate_strings() {
    yield strtoupper('Hello');
    yield ' ';
    yield strtoupper('World!');
    return strtoupper('UNUSED');
}

// The result of strtolower is locale-dependent, meaning that it cannot be converted to a constant by opcache. Also, test reference counting.
dump_fold(new ArrayObject([]));
dump_fold(new ArrayObject([]), function () {}, strtolower('TEST'));
dump_fold(new ArrayObject(['x', 'y', 'z']), function ($carry, $item) { $carry .= $item; return $carry; }, strtolower('TEST'));
dump_fold(new ArrayObject([strtolower('WORLD'), '!']), function ($carry, $item) { $carry .= $item; return $carry; }, strtolower('HELLO'));
dump_fold(new ArrayObject([strtolower('WORLD')]), function (string $carry, string $item): string { return $carry . $item; }, strtolower('HELLO'));
dump_fold(generate_strings(), function (string $carry, string $item): string { return $carry . $item; }, '');
dump_fold(generate_strings(), function ($carry, $item): string { $item = $carry . $item; unset($carry);return $item; }, '');
// Passing by reference is not supported.
dump_fold(generate_strings(), function (string &$carry, string $item): string { $carry .= $item; return $carry;}, '');

?>
--EXPECTF--
Caught ArgumentCountError: Teds\fold() expects exactly 3 arguments, 1 given
string(4) "test"
string(7) "testxyz"
string(11) "helloworld!"
string(10) "helloworld"
string(12) "HELLO WORLD!"
string(12) "HELLO WORLD!"

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in %s on line 12

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in %s on line 12

Warning: {closure}(): Argument #1 ($carry) must be passed by reference, value given in %s on line 12
string(12) "HELLO WORLD!"
