--TEST--
Teds\IntVector is an IteratorAggregate
--FILE--
<?php

$vector = new Teds\IntVector([9001, -50000]);
foreach ($vector as $k => $v) {
    foreach ($vector as $k2 => $v2) {
        printf("k=%s k2=%s v=%s v2=%s\n", json_encode($k), json_encode($k2), json_encode($v), json_encode($v2));
    }
}
?>
--EXPECT--
k=0 k2=0 v=9001 v2=9001
k=0 k2=1 v=9001 v2=-50000
k=1 k2=0 v=-50000 v2=9001
k=1 k2=1 v=-50000 v2=-50000