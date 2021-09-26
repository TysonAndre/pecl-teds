--TEST--
Teds\Vector isEmpty()
--FILE--
<?php
// discards keys
var_dump((new Teds\Vector())->isEmpty());
var_dump((new Teds\Vector([]))->isEmpty());
var_dump((new Teds\Vector([false]))->isEmpty());
var_dump((new Teds\Vector([1,2,3,4]))->isEmpty());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
