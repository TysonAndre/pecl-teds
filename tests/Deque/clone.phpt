--TEST--
Teds\Deque can be cloned after building properties table.
--FILE--
<?php
$x = new Teds\Deque([new stdClass()]);
var_dump($x);
$y = clone $x;
var_dump($y);
?>
--EXPECT--
object(Teds\Deque)#1 (1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
object(Teds\Deque)#3 (1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
