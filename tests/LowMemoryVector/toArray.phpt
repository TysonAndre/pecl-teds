--TEST--
Teds\LowMemoryVector toArray()
--FILE--
<?php

$it = new Teds\LowMemoryVector([new stdClass()]);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Teds\LowMemoryVector([]);
var_dump($it->toArray());
var_dump($it->toArray());
var_dump(Teds\LowMemoryVector::__set_state([])->toArray());
var_dump(Teds\LowMemoryVector::__set_state([(object)[]])->toArray());
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
array(0) {
}
array(1) {
  [0]=>
  object(stdClass)#1 (0) {
  }
}