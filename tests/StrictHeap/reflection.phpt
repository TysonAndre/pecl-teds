--TEST--
Teds\StrictMinHeap and StrictMaxHeap reflection
--FILE--
<?php
foreach (['Teds\StrictMinHeap', 'Teds\StrictMaxHeap'] as $class) {
    printf("%s: parent=%s\n", $class, get_parent_class($class));
}
?>
--EXPECT--
Teds\StrictMinHeap: parent=
Teds\StrictMaxHeap: parent=