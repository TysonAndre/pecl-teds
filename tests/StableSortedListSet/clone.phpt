--TEST--
Teds\StableSortedListSet can be cloned
--FILE--
<?php

$it = new Teds\StableSortedListSet([new stdClass(), new ArrayObject()]);
$it2 = clone $it;
unset($it);
foreach ($it2 as $value) {
    var_dump($value);
}
?>
--EXPECT--
object(stdClass)#2 (0) {
}
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}