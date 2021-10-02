--TEST--
Test array_value_last()/array_value_first() function
--FILE--
<?php

use function Teds\array_value_first;
use function Teds\array_value_last;

// Something opcache won't optimize to a constant, for reference counting checks
$x = [];
for ($i = 0; $i < 2; $i++) { $x[$i] = "x$i"; }

var_dump(array_value_first($x));
var_dump(array_value_last($x));
unset($x[0]);
var_dump(array_value_first($x));
var_dump(array_value_last($x));
var_dump($x);
unset($x[1]);
var_dump(array_value_first($x));
var_dump(array_value_last($x));
var_dump(array_value_first([]));
var_dump(array_value_last([]));
var_dump(array_value_first(['key' => new stdClass()]));
var_dump(array_value_last(['key' => new stdClass()]));

$obj = new ArrayObject();
$ref = [&$obj];
var_dump(array_value_first($ref));
var_dump(array_value_last($ref));
?>
--EXPECT--
string(2) "x0"
string(2) "x1"
string(2) "x1"
string(2) "x1"
array(1) {
  [1]=>
  string(2) "x1"
}
NULL
NULL
NULL
NULL
object(stdClass)#1 (0) {
}
object(stdClass)#1 (0) {
}
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
