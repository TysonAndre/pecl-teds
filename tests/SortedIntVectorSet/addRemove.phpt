--TEST--
Teds\SortedIntVectorSet add/remove
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
$it = new Teds\SortedIntVectorSet([]);
$it->add(1);
$it->add(0);
$it->add(2);
var_dump($it->add(-1));
var_dump($it->add(-1));
var_dump($it);
var_dump($it->remove(0));
var_dump($it->remove(0));
var_dump($it);
$it->add(12345678);
$it->add(-9876654);
$it->remove(0);
var_dump($it);
?>
--EXPECT--
bool(true)
bool(false)
object(Teds\SortedIntVectorSet)#1 (4) {
  [0]=>
  int(-1)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
bool(false)
bool(false)
object(Teds\SortedIntVectorSet)#1 (3) {
  [0]=>
  int(-1)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
object(Teds\SortedIntVectorSet)#1 (5) {
  [0]=>
  int(-9876654)
  [1]=>
  int(-1)
  [2]=>
  int(1)
  [3]=>
  int(2)
  [4]=>
  int(12345678)
}
