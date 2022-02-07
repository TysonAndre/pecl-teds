--TEST--
Teds\IntVector to array
--FILE--
<?php
// discards keys
$it = new Teds\IntVector([123_456_789]);
var_dump((array)$it);
$it[0] = -1;
var_dump((array)$it);
var_dump($it);
$it->pop();
var_dump($it);

?>
--EXPECT--
array(1) {
  [0]=>
  int(123456789)
}
array(1) {
  [0]=>
  int(-1)
}
object(Teds\IntVector)#1 (1) {
  [0]=>
  int(-1)
}
object(Teds\IntVector)#1 (0) {
}