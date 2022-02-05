--TEST--
Teds\LowMemoryVector constructed from array
--FILE--
<?php
// discards keys
$it = new Teds\LowMemoryVector(['first' => 'x', 'second' => new stdClass()]);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
var_dump((array)$it);

$it = new Teds\LowMemoryVector([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

// Teds\LowMemoryVector will always reindex keys in the order of iteration, like array_values() does.
$it = new Teds\LowMemoryVector([2 => 'a', 0 => 'b']);
var_dump($it);

var_dump(new Teds\LowMemoryVector([-1 => new stdClass()]));
?>
--EXPECT--
Key: 0
Value: 'x'
Key: 1
Value: (object) array(
)
object(Teds\LowMemoryVector)#1 (2) {
  [0]=>
  string(1) "x"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
object(Teds\LowMemoryVector)#3 (0) {
}
array(0) {
}
object(Teds\LowMemoryVector)#1 (2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
object(Teds\LowMemoryVector)#3 (1) {
  [0]=>
  object(stdClass)#4 (0) {
  }
}