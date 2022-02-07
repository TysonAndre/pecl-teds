--TEST--
Teds\IntVector isEmpty()
--FILE--
<?php
// discards keys
var_dump((new Teds\IntVector())->isEmpty());
var_dump((new Teds\IntVector([]))->isEmpty());
var_dump((new Teds\IntVector([1]))->isEmpty());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
