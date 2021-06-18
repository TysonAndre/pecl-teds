--TEST--
Teds\Vector toArray()
--FILE--
<?php

$it = new Teds\Vector(['first' => new stdClass()]);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Teds\Vector([]);
var_dump($it->toArray());
var_dump($it->toArray());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(0) {
}
array(0) {
}