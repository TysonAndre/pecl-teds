--TEST--
Teds\BitVector constructed from array
--FILE--
<?php
// discards keys
$it = new Teds\BitVector(['first' => true, 'second' => false]);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
var_dump((array)$it);

$it = new Teds\BitVector([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

// Teds\BitVector will always reindex keys in the order of iteration, like array_values() does.
$it = new Teds\BitVector([2 => true, 0 => false]);
var_dump($it);

var_dump(new Teds\BitVector([-1 => false]));
?>
--EXPECT--
Key: 0
Value: true
Key: 1
Value: false
object(Teds\BitVector)#1 (2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
object(Teds\BitVector)#2 (0) {
}
array(0) {
}
object(Teds\BitVector)#1 (2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
object(Teds\BitVector)#2 (1) {
  [0]=>
  bool(false)
}