--TEST--
Teds\Vector setSize with $default
--FILE--
<?php
function show(Teds\Vector $it) {
    printf("value=%s count=%d capacity=%d\n", json_encode($it), $it->count(), $it->capacity());
}

$it = new Teds\Vector();
show($it);
$it->setSize(2, 0);
$it[0] = new stdClass();
$it->setSize(3, null);
show($it);
$it->setSize(0, new stdClass());
show($it);
$it->setSize(5, strtoupper('na'));
show($it);
?>
--EXPECT--
value=[] count=0 capacity=0
value=[{},0,null] count=3 capacity=3
value=[] count=0 capacity=0
value=["NA","NA","NA","NA","NA"] count=5 capacity=5