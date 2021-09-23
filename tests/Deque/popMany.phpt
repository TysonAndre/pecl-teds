--TEST--
Teds\Deque push repeatedly
--FILE--
<?php

$dq = new Teds\Deque();
for ($i = 0; $i < 8; $i++) {
    $dq->push("v$i");
}
$dq->shift();
$dq->push("extra");
printf("dq=%s count=%d capacity=%d\n", json_encode($dq), $dq->count(), $dq->capacity());
for ($i = 0; $i < 7; $i++) {
    $value = $dq->shift();
    printf("popped %s capacity=%d\n", $value, $dq->capacity());
}
// Deque should reclaim memory once roughly a quarter of the memory is actually used.
printf("dq=%s count=%d capacity=%d\n", json_encode($dq), $dq->count(), $dq->capacity());
var_dump($dq);
$dq->clear();
printf("dq=%s count=%d capacity=%d\n", json_encode($dq), $dq->count(), $dq->capacity());
var_dump($dq);
?>
--EXPECT--
dq=["v1","v2","v3","v4","v5","v6","v7","extra"] count=8 capacity=8
popped v1 capacity=8
popped v2 capacity=8
popped v3 capacity=8
popped v4 capacity=8
popped v5 capacity=8
popped v6 capacity=8
popped v7 capacity=4
dq=["extra"] count=1 capacity=4
object(Teds\Deque)#1 (1) {
  [0]=>
  string(5) "extra"
}
dq=[] count=0 capacity=0
object(Teds\Deque)#1 (0) {
}