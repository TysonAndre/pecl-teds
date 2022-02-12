--TEST--
Teds\BitSet isEmpty()
--FILE--
<?php
// discards keys
var_dump((new Teds\BitSet())->isEmpty());
var_dump((new Teds\BitSet([]))->isEmpty());
var_dump((new Teds\BitSet([true]))->isEmpty());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
