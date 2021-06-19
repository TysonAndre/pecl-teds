--TEST--
Teds\Deque toArray()
--FILE--
<?php

$it = new Teds\Deque(['first' => new stdClass()]);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Teds\Deque([]);
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