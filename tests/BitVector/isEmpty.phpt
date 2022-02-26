--TEST--
Teds\BitVector isEmpty()
--FILE--
<?php
// discards keys
var_dump((new Teds\BitVector())->isEmpty());
var_dump((new Teds\BitVector([]))->isEmpty());
var_dump((new Teds\BitVector([true]))->isEmpty());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
