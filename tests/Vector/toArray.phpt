--TEST--
Teds\Vector toArray()
--FILE--
<?php

$it = new Teds\Vector([new stdClass()]);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Teds\Vector([]);
var_dump($it->toArray());
var_dump($it->toArray());
$it->setSize(2, strtoupper('test'));
var_dump($it->toArray());
var_dump($it->toArray());
var_dump(Teds\Vector::__set_state([])->toArray());
var_dump(Teds\Vector::__set_state([(object)[]])->toArray());
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
array(2) {
  [0]=>
  string(4) "TEST"
  [1]=>
  string(4) "TEST"
}
array(2) {
  [0]=>
  string(4) "TEST"
  [1]=>
  string(4) "TEST"
}
array(0) {
}
array(1) {
  [0]=>
  object(stdClass)#1 (0) {
  }
}