--TEST--
Teds\StrictMinHeap and StrictMaxHeap not extensible
--FILE--
<?php
class ExtendedStrictMinHeap extends Teds\StrictMinHeap {
}
?>
--EXPECTF--
Fatal error: Class ExtendedStrictMinHeap %s final class %s