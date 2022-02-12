--TEST--
Teds\SortedStrictSet floating point edge cases
--FILE--
<?php

function create_value(string $x) { return "value_$x"; }

$it = new Teds\SortedStrictSet([]);
$it->add(0.0);
$it->add(-0.0);
echo "Add array of signed zero\n";
$it->add([0.0]);
$it->add([-0.0]);
$it->add(0);
echo "Add nan\n";
$it->add(NAN);
$zero = 0;
$it->add(fdiv($zero, $zero));

$it->add([NAN]);
$it->add([fdiv($zero, $zero)]);
var_dump($it);
?>
--EXPECT--
Add array of signed zero
Add nan
object(Teds\SortedStrictSet)#1 (5) {
  [0]=>
  int(0)
  [1]=>
  float(0)
  [2]=>
  float(NAN)
  [3]=>
  array(1) {
    [0]=>
    float(0)
  }
  [4]=>
  array(1) {
    [0]=>
    float(NAN)
  }
}