--TEST--
Teds\StableMinHeap and StableMaxHeap reflection
--FILE--
<?php
foreach (['Teds\StableMinHeap', 'Teds\StableMaxHeap'] as $class) {
    printf("%s: parent=%s\n", $class, get_parent_class($class));
}
?>
--EXPECT--
Teds\StableMinHeap: parent=SplHeap
Teds\StableMaxHeap: parent=SplHeap