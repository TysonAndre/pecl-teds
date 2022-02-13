--TEST--
Test fold() function
--FILE--
<?php

use function Teds\fold;

/*
    Prototype: mixed Teds\fold(array $array, callable($carry, $item): mixed $callback);
    Description: Iterate over iterable and fold
*/

function dump_fold(iterable $values, ...$args) {
    try {
        var_dump(fold($values, ...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

// The result of strtolower is locale-dependent, meaning that it cannot be converted to a constant by opcache.
dump_fold([]);
dump_fold([], function () {}, strtolower('TEST'));
dump_fold(['x', 'y', 'z'], function ($carry, $item) { $carry .= $item; return $carry; }, strtolower('TEST'));
dump_fold([strtolower('WORLD'), '!'], function ($carry, $item) { $carry .= $item; return $carry; }, strtolower('HELLO'));
dump_fold([strtolower('WORLD')], function (string $carry, string $item): string { return $carry . $item; }, strtolower('HELLO'));

?>
--EXPECT--
Caught ArgumentCountError: Teds\fold() expects exactly 3 arguments, 1 given
string(4) "test"
string(7) "testxyz"
string(11) "helloworld!"
string(10) "helloworld"
