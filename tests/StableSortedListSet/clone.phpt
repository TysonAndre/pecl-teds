--TEST--
Teds\StableSortedListSet can be cloned
--FILE--
<?php

$it = new Teds\StableSortedListSet([new stdClass(), new ArrayObject(), null, true]);
foreach ($it as $value) {
    var_dump($value);
}
$it2 = clone $it;
unset($it);
foreach ($it2 as $value) {
    var_dump($value);
}
?>
--EXPECT--
NULL
bool(true)
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(stdClass)#2 (0) {
}
NULL
bool(true)
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(stdClass)#2 (0) {
}