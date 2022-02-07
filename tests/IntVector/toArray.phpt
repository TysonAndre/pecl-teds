--TEST--
Teds\IntVector toArray()
--FILE--
<?php

$it = new Teds\IntVector([123456789]);
var_dump($it->toArray());
$it = new Teds\IntVector([]);
var_dump($it->toArray());
var_dump(Teds\IntVector::__set_state([])->toArray());
var_dump(Teds\IntVector::__set_state([-1234567])->toArray());
?>
--EXPECT--
array(1) {
  [0]=>
  int(123456789)
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  int(-1234567)
}