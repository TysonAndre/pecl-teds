--TEST--
Teds\BitSet to array
--FILE--
<?php
// discards keys
$it = new Teds\BitSet([true, false]);
var_dump($it->toArray());
var_dump($it->count());
$it[0] = false;
$it[1] = true;
var_dump((array)$it);
var_dump($it);

?>
--EXPECT--
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
int(2)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
object(Teds\BitSet)#1 (2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}