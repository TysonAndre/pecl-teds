--TEST--
Test strict_hash() function on recursive arrays
--SKIPIF--
<?php if (PHP_INT_SIZE < 8) echo "skip 64-bit only\n"; ?>
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
// NOTE: This does not attempt to distinguish between different recursive arrays or different levels of recursion.
// Objects with different values are allowed to have the same hash,
// and this is an uncommon use case.
var_dump(Teds\strict_hash($values));
?>
--EXPECT--
array(1) {
  [0]=>
  *RECURSION*
}
int(-2751415950963617167)
int(8622596786386105489)
array(1) {
  [0]=>
  *RECURSION*
}
int(-2751415950963617167)
int(8622596786386105489)
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
int(7301983740570972493)
int(7301983740570972493)