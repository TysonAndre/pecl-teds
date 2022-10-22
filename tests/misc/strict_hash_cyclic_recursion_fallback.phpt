--TEST--
Test strict_hash() handling of cyclic recursion edge cases
--SKIPIF--
<?php if (PHP_INT_SIZE < 8) echo "skip 64-bit only\n"; ?>
--FILE--
<?php

$x = [];
$x[1] = &$x;
$y = [1 => $x];

var_dump($x === $y); // true, so we expect the strict_hash to be the same. Only hash the key 1 but don't hash the top level array

// Expected: same hashes are computed
// Observed: Different hashes in Teds 1.2.6
var_dump(Teds\strict_hash($x), Teds\strict_hash($y));
var_dump($x);
echo "Test hash skips hashing sub-arrays if array contains cyclic arrays anywhere\n";
$x[2] = [];
$x[1] = &$x;
var_dump(Teds\strict_hash($x));
$x[2][] = 1;
var_dump(Teds\strict_hash($x));
var_dump($x);
$y[2] = [1];
$y[1] = &$x;
echo "x, y values:\n";
var_dump($x);
var_dump($y);
echo "x === y: ";
var_export($x === $y);
echo "\n";
var_dump(Teds\strict_hash($x));
var_dump(Teds\strict_hash($y));
$other = [];
$other[1] = &$other;
var_dump(new Teds\StrictHashSet([$x, $y, null, &$other, $other, [1 => null]]));
?>
--EXPECT--
bool(true)
int(-5561530578556069188)
int(-5561530578556069188)
array(1) {
  [1]=>
  *RECURSION*
}
Test hash skips hashing sub-arrays if array contains cyclic arrays anywhere
int(7251907892736144760)
int(7251907892736144760)
array(2) {
  [1]=>
  *RECURSION*
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
}
x, y values:
array(2) {
  [1]=>
  *RECURSION*
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(2) {
  [1]=>
  &array(2) {
    [1]=>
    *RECURSION*
    [2]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
}
x === y: true
int(7251907892736144760)
int(7251907892736144760)
object(Teds\StrictHashSet)#1 (4) {
  [0]=>
  array(2) {
    [1]=>
    *RECURSION*
    [2]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
  [1]=>
  NULL
  [2]=>
  array(1) {
    [1]=>
    *RECURSION*
  }
  [3]=>
  array(1) {
    [1]=>
    NULL
  }
}