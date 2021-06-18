--TEST--
Teds\ImmutableSequence toArray()
--FILE--
<?php

$it = new Teds\ImmutableSequence(['first' => new stdClass()]);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Teds\ImmutableSequence([]);
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