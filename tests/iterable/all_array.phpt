--TEST--
Test all() function
--FILE--
<?php

use function Teds\all;

/*
    Prototype: bool all(array $array, mixed $callback);
    Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($item)
{
    return is_int($item);
}

echo "\n*** Testing not enough or wrong arguments ***\n";

function dump_all(...$args) {
    try {
        var_dump(all(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

dump_all();
dump_all(true);
dump_all([]);
dump_all(true, function () {});
dump_all([], true);

echo "\n*** Testing basic functionality ***\n";

dump_all([1, 2, 3], 'is_int_ex');
dump_all(['hello', 1, 2, 3], 'is_int_ex');
$iterations = 0;
dump_all(['hello', 1, 2, 3], function($item) use (&$iterations) {
    ++$iterations;
    return is_int($item);
});
var_dump($iterations);

echo "\n*** Testing edge cases ***\n";

dump_all([], 'is_int_ex');

echo "\nDone";
?>
--EXPECTF--
*** Testing not enough or wrong arguments ***
Caught ArgumentCountError: Teds\all() expects at least 1 argument, 0 given
Caught TypeError: Teds\all(): Argument #1 ($iterable) must be of type %s, bool given
bool(true)
Caught TypeError: Teds\all(): Argument #1 ($iterable) must be of type %s, bool given
Caught TypeError: Teds\all(): Argument #2 ($callback) must be a valid callback%S, no array or string given

*** Testing basic functionality ***
bool(true)
bool(false)
bool(false)
int(1)

*** Testing edge cases ***
bool(true)

Done