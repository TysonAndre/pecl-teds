--TEST--
Teds\Deque insert and remove at front offset is efficient
--FILE--
<?php
const N = 100000;

$deque = new Teds\Deque();
$start = microtime(true);
for ($i = 0; $i < N; $i++) {
    $deque->insert(0, $i % 10);
}
$mid = microtime(true);
$total = 0;
while (count($deque) > 0) {
    $total += $deque[0];
    unset($deque[0]);
}
$end = microtime(true);
printf("repeated insert: %.3f\n", $mid - $start);
printf("repeated read+remove: %.3f\n", $end - $mid);
printf("total: %d\n", $total);
?>
--EXPECTF--
repeated insert: %f
repeated read+remove: %f
total: 450000
