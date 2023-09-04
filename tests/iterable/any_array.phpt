--TEST--
Test any() function
--FILE--
<?php

use function Teds\any;

/*
	Prototype: bool any(array $iterable, mixed $callback);
	Description: Iterate array and stop based on return value of callback
*/

function is_int_ex($nr)
{
	return is_int($nr);
}

echo "\n*** Testing not enough or wrong arguments ***\n";

function dump_any(...$args) {
    try {
        var_dump(any(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

dump_any();
dump_any(true);
dump_any([]);
dump_any(true, function () {});
dump_any([], true);

echo "\n*** Testing basic functionality ***\n";

dump_any(['hello', 'world'], 'is_int_ex');
dump_any(['hello', 1, 2, 3], 'is_int_ex');
$iterations = 0;
dump_any(['hello', 1, 2, 3], function($item) use (&$iterations) {
	++$iterations;
	return is_int($item);
});
var_dump($iterations);

echo "\n*** Testing second argument to predicate ***\n";

dump_any([1, 2, 3], function($item, $key) {
	var_dump($key);
	return false;
});

echo "\n*** Testing edge cases ***\n";

dump_any([], 'is_int_ex');

dump_any(['key' => 'x'], null);

echo "\nDone";
?>
--EXPECTF--
*** Testing not enough or wrong arguments ***
Caught ArgumentCountError: Teds\any() expects at least 1 argument, 0 given
Caught TypeError: Teds\any(): Argument #1 ($iterable) must be of type %s, %s given
bool(false)
Caught TypeError: Teds\any(): Argument #1 ($iterable) must be of type %s, %s given
Caught TypeError: Teds\any(): Argument #2 ($callback) must be a valid callback%S, no array or string given

*** Testing basic functionality ***
bool(false)
bool(true)
bool(true)
int(2)

*** Testing second argument to predicate ***
Caught ArgumentCountError: Too few arguments to function {closure}(), 1 passed and exactly 2 expected

*** Testing edge cases ***
bool(false)
bool(true)

Done
