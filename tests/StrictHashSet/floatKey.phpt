--TEST--
Teds\StrictHashSet floating point edge cases
--FILE--
<?php

function create_value(string $x) { return "value_$x"; }

$it = new Teds\StrictHashSet([]);
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
object(Teds\StrictHashSet)#1 (5) {
  [0]=>
  float(0)
  [1]=>
  array(1) {
    [0]=>
    float(0)
  }
  [2]=>
  int(0)
  [3]=>
  float(NAN)
  [4]=>
  array(1) {
    [0]=>
    float(NAN)
  }
}