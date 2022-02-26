--TEST--
Teds\BitVector is an IteratorAggregate
--FILE--
<?php

$vector = new Teds\BitVector([true, false]);
foreach ($vector as $k => $v) {
    foreach ($vector as $k2 => $v2) {
        printf("k=%s k2=%s v=%s v2=%s\n", json_encode($k), json_encode($k2), json_encode($v), json_encode($v2));
    }
}
?>
--EXPECT--
k=0 k2=0 v=true v2=true
k=0 k2=1 v=true v2=false
k=1 k2=0 v=false v2=true
k=1 k2=1 v=false v2=false