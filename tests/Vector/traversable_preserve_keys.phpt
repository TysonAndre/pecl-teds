--TEST--
Vector constructed from Traversable preserves keys
--FILE--
<?php

use Teds\Vector;

function yields_values() {
    yield 5 => (object)['key' => 'value'];
    yield 0 => new stdClass();;
    yield 0 => true;
    yield (object)[] => true;
}

// Vector eagerly evaluates the passed in Traversable
$it = new Vector(yields_values());
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
unset($it);

?>
--EXPECT--
Key: 0
Value: (object) array(
   'key' => 'value',
)
Key: 1
Value: (object) array(
)
Key: 2
Value: true
Key: 3
Value: true
count=4 capacity=4