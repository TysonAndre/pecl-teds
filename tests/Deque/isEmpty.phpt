--TEST--
Teds\Deque isEmpty()
--FILE--
<?php
// discards keys
var_dump((new Teds\Deque())->isEmpty());
var_dump((new Teds\Deque([]))->isEmpty());
var_dump((new Teds\Deque([false]))->isEmpty());
var_dump((new Teds\Deque([1,2,3,4]))->isEmpty());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
