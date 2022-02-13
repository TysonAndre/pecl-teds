--TEST--
Teds\IntVector constructed from array
--FILE--
<?php
// discards keys
$it = new Teds\IntVector(['first' => 100, 'second' => -1]);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
var_dump((array)$it);

$it = new Teds\IntVector([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

// Teds\IntVector will always reindex keys in the order of iteration, like array_values() does.
$it = new Teds\IntVector([2 => 10000, 0 => 10001]);
var_dump($it);

var_dump(new Teds\IntVector([-1 => 1]));
?>
--EXPECT--
Key: 0
Value: 100
Key: 1
Value: -1
object(Teds\IntVector)#1 (2) {
  [0]=>
  int(100)
  [1]=>
  int(-1)
}
array(2) {
  [0]=>
  int(100)
  [1]=>
  int(-1)
}
object(Teds\IntVector)#2 (0) {
}
array(0) {
}
object(Teds\IntVector)#1 (2) {
  [0]=>
  int(10000)
  [1]=>
  int(10001)
}
object(Teds\IntVector)#2 (1) {
  [0]=>
  int(1)
}