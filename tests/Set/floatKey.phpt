--TEST--
Teds\Set floating point edge cases
--FILE--
<?php

function create_value(string $x) { return "value_$x"; }
function test_set_floating_point(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name([]);
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
}
foreach ([
    Teds\StrictTreeSet::class,
    Teds\StrictHashSet::class,
    Teds\StrictSortedVectorSet::class,
] as $class_name) {
    test_set_floating_point($class_name);
}
?>
--EXPECT--
Test Teds\StrictTreeSet
Add array of signed zero
Add nan
object(Teds\StrictTreeSet)#1 (5) {
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
Test Teds\StrictHashSet
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
Test Teds\StrictSortedVectorSet
Add array of signed zero
Add nan
object(Teds\StrictSortedVectorSet)#1 (5) {
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