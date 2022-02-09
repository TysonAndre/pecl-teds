--TEST--
Teds\StableMinHeap and StableMaxHeap not extensible
--FILE--
<?php
class ExtendedStableMinHeap extends Teds\StableMinHeap {
}
?>
--EXPECTF--
Fatal error: Class ExtendedStableMinHeap %s final class %s