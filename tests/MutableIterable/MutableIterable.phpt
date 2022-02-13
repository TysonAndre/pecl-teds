--TEST--
Teds\MutableIterable constructed from array
--FILE--
<?php

$it = new Teds\MutableIterable(['first' => 'x', 'second' => new stdClass()]);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
// TODO: Change var_dump representation to [['key' => $key, 'value' => $value],...]
var_dump($it);
var_dump((array)$it);

$it = new Teds\MutableIterable([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

?>
--EXPECT--
Key: 'first'
Value: 'x'
Key: 'second'
Value: (object) array(
)
object(Teds\MutableIterable)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "second"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "second"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
object(Teds\MutableIterable)#3 (0) {
}
array(0) {
}