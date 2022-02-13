--TEST--
Teds\StrictHashMap converted to array.
--FILE--
<?php

$it = new Teds\StrictHashMap();
var_dump((array) $it);
$o1 = new stdClass();
$o2 = new stdClass();
$it[$o1] = $o2;
var_dump((array) $it);
$it[$o1] = null;
var_dump((array) $it);
unset($it[$o1]);
var_dump((array) $it);
var_dump($it);
?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    NULL
  }
}
array(0) {
}
object(Teds\StrictHashMap)#1 (0) {
}