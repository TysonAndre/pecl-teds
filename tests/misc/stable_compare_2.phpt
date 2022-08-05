--TEST--
Test stable_compare() function extended
--FILE--
<?php

class A {}
class B {}

function dump_stable_compare($a, $b) {
    printf("stable_compare(%s, %s): %d (opposite %d)\n", ltrim(var_export($a, true), '\\'), ltrim(var_export($b, true), '\\'), Teds\stable_compare($a, $b), Teds\stable_compare($b, $a));
}

$a = new A();
$b = new B();
$values = [
    (object)[],
    $b,
    $a,
    new ArrayObject(),
    $a,
    new SplObjectStorage(),
    new ArrayObject([]),
    (object)['k' => 1],  // keys of objects are ignored, this is sorted by object handle for objects of the same type
];
usort($values, Closure::fromCallable('Teds\stable_compare'));
var_dump($values);
for ($i = 1; $i < count($values); $i++) {
    dump_stable_compare($values[$i - 1], $values[$i]);
}
?>
--EXPECT--
array(8) {
  [0]=>
  object(A)#1 (0) {
  }
  [1]=>
  object(A)#1 (0) {
  }
  [2]=>
  object(ArrayObject)#4 (1) {
    ["storage":"ArrayObject":private]=>
    array(0) {
    }
  }
  [3]=>
  object(ArrayObject)#6 (1) {
    ["storage":"ArrayObject":private]=>
    array(0) {
    }
  }
  [4]=>
  object(B)#2 (0) {
  }
  [5]=>
  object(SplObjectStorage)#5 (1) {
    ["storage":"SplObjectStorage":private]=>
    array(0) {
    }
  }
  [6]=>
  object(stdClass)#3 (0) {
  }
  [7]=>
  object(stdClass)#7 (1) {
    ["k"]=>
    int(1)
  }
}
stable_compare(A::__set_state(array(
)), A::__set_state(array(
))): 0 (opposite 0)
stable_compare(A::__set_state(array(
)), ArrayObject::__set_state(array(
))): -1 (opposite 1)
stable_compare(ArrayObject::__set_state(array(
)), ArrayObject::__set_state(array(
))): -1 (opposite 1)
stable_compare(ArrayObject::__set_state(array(
)), B::__set_state(array(
))): -1 (opposite 1)
stable_compare(B::__set_state(array(
)), SplObjectStorage::__set_state(array(
))): -1 (opposite 1)
stable_compare(SplObjectStorage::__set_state(array(
)), (object) array(
)): -1 (opposite 1)
stable_compare((object) array(
), (object) array(
   'k' => 1,
)): -1 (opposite 1)