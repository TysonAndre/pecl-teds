--TEST--
Teds\Deque shift
--FILE--
<?php

$dq = new Teds\Deque([new stdClass(), strtoupper('test')]);
var_dump($dq->shift());
var_dump($dq[0]);
?>
--EXPECT--
object(stdClass)#2 (0) {
}
string(4) "TEST"