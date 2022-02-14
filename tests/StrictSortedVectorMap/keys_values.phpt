--TEST--
Teds\StrictSortedVectorMap keys() and values()
--FILE--
<?php

$it = new Teds\StrictSortedVectorMap(['first' => new stdClass()]);
var_dump($it->keys());
var_dump($it->values());
$it = new Teds\StrictSortedVectorMap([]);
var_dump($it->keys());
var_dump($it->values());
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "first"
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