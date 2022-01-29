--TEST--
Teds\SortedStrictMap insert during iteration
--FILE--
<?php

function create_key(int $i) {
    return "x$i";
}
$map = new Teds\SortedStrictMap([create_key(0) => 'x', create_key(2) => create_key(100), create_key(5) => 'a']);
foreach ($map as $key => $value) {
    $map[create_key(1)] = create_key(123);
    $map[create_key(2)] = create_key(124);
    $map[create_key(9)] = create_key(125);
    printf("Key: %s\nValue: %s\n", $key, $value);
}
var_dump($map);
?>
--EXPECT--
Key: x0
Value: x
Key: x1
Value: x123
Key: x2
Value: x124
Key: x5
Value: a
Key: x9
Value: x125
object(Teds\SortedStrictMap)#1 (5) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "x0"
    [1]=>
    string(1) "x"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "x1"
    [1]=>
    string(4) "x123"
  }
  [2]=>
  array(2) {
    [0]=>
    string(2) "x2"
    [1]=>
    string(4) "x124"
  }
  [3]=>
  array(2) {
    [0]=>
    string(2) "x5"
    [1]=>
    string(1) "a"
  }
  [4]=>
  array(2) {
    [0]=>
    string(2) "x9"
    [1]=>
    string(4) "x125"
  }
}