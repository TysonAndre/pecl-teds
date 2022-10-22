--TEST--
Test strict_hash() function on recursive arrays
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) echo "skip 32-bit only\n"; ?>
--FILE--
<?php

$values = [];
// Should hash the same way for different but equivalent array instances
for ($i = 0; $i < 2; $i++) {
    $x = [];
    $x[0] = &$x;
    var_dump($x);
    var_dump(Teds\strict_hash($x));
    $x[] = 123;
    var_dump(Teds\strict_hash($x));
    $values[] = &$x;
    unset($x);
}
echo "Recursive arrays hashed in a predictable way\n";
var_dump($values);
var_dump(Teds\strict_hash($values));
$values[0] = &$values[1];
// NOTE: This currently distinguishes between different levels of recursion.
var_dump(Teds\strict_hash($values));
echo "Test deeper recursion level\n";
$values[0][0] = &$values;

var_dump(Teds\strict_hash($values));
?>
--EXPECT--
array(1) {
  [0]=>
  *RECURSION*
}
int(-665309858)
int(-351226279)
array(1) {
  [0]=>
  *RECURSION*
}
int(-665309858)
int(-351226279)
Recursive arrays hashed in a predictable way
array(2) {
  [0]=>
  &array(2) {
    [0]=>
    *RECURSION*
    [1]=>
    int(123)
  }
  [1]=>
  &array(2) {
    [0]=>
    *RECURSION*
    [1]=>
    int(123)
  }
}
int(-1313776964)
int(-1313776964)
Test deeper recursion level
int(-1313776964)