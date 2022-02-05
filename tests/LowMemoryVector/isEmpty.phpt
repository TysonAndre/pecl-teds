--TEST--
Teds\LowMemoryVector isEmpty()
--FILE--
<?php
// discards keys
var_dump((new Teds\LowMemoryVector())->isEmpty());
var_dump((new Teds\LowMemoryVector([]))->isEmpty());
var_dump((new Teds\LowMemoryVector([false]))->isEmpty());
var_dump((new Teds\LowMemoryVector([1,2,3,4]))->isEmpty());
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
