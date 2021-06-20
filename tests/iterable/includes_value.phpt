--TEST--
Test includes_value() function
--FILE--
<?php

use function Teds\includes_value;

/*
	Prototype: bool includes_value(array $iterable, mixed $value);
	Description: Iterate array and stop and return true once a value includes $value.
    Similar to in_array($iterable, $value, true) but also works on iterables.
*/

function is_int_ex($nr)
{
	return is_int($nr);
}

echo "\n*** Testing not enough or wrong arguments ***\n";

function dump_includes_value(...$args) {
    try {
        printf("For %s: ", json_encode($args));
        var_dump(includes_value(...$args));
    } catch (Error $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

// use strtoupper to create (locale-dependent) temporary strings to test reference counting.
dump_includes_value();
dump_includes_value([], null);
dump_includes_value([null], null);
dump_includes_value([null], strtoupper('test'));
dump_includes_value([strtoupper('fail'), strtoupper('test')], strtoupper('test'));
dump_includes_value(new ArrayObject([strtoupper('fail'), strtoupper('test')]), strtoupper('test'));
dump_includes_value(new ArrayObject([strtoupper('fail'), strtoupper('test')]), strtoupper('missing'));
dump_includes_value((new ArrayObject([strtoupper('fail'), strtoupper('test')]))->getIterator(), strtoupper('missing'));
dump_includes_value(new ArrayObject([]), strtoupper('missing'));
?>
--EXPECT--
*** Testing not enough or wrong arguments ***
For []: Caught ArgumentCountError: Teds\includes_value() expects exactly 2 arguments, 0 given
For [[],null]: bool(false)
For [[null],null]: bool(true)
For [[null],"TEST"]: bool(false)
For [["FAIL","TEST"],"TEST"]: bool(true)
For [{"0":"FAIL","1":"TEST"},"TEST"]: bool(true)
For [{"0":"FAIL","1":"TEST"},"MISSING"]: bool(false)
For [{"0":"FAIL","1":"TEST"},"MISSING"]: bool(false)
For [{},"MISSING"]: bool(false)
