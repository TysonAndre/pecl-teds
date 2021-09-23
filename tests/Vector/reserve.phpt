--TEST--
Teds\Vector reserve()
--FILE--
<?php
$it = new Teds\Vector();
$it->reserve(2);
printf("values=%s count=%d capacity=%d\n", json_encode($it), $it->count(), $it->capacity());
$it[] = strtoupper('first');
$it[] = 'second';
printf("values=%s count=%d capacity=%d\n", json_encode($it), $it->count(), $it->capacity());
$it->reserve(3);
printf("capacity=%d\n", $it->capacity());
$it[] = 'extra';
printf("values=%s count=%d capacity=%d\n", json_encode($it), $it->count(), $it->capacity());
// reserve does nothing if the capacity is already greater than the requested capacity
$it->reserve(3);
$it->reserve(1);
$it->reserve(0);
$it->reserve(PHP_INT_MIN);
printf("unchanged values=%s count=%d capacity=%d\n", json_encode($it), $it->count(), $it->capacity());
$it->reserve(PHP_INT_MAX);
echo "Unreachable\n";
?>
--EXPECTF--
values=[] count=0 capacity=2
values=["FIRST","second"] count=2 capacity=2
capacity=3
values=["FIRST","second","extra"] count=3 capacity=3
unchanged values=["FIRST","second","extra"] count=3 capacity=3

Fatal error: exceeded max valid Teds\Vector capacity in %s on line 18
