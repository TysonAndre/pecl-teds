--TEST--
Teds\BitSet offsetSet/set
--FILE--
<?php

function expect_throws(Closure $cb): void {
    try {
        $cb();
        echo "Unexpectedly didn't throw\n";
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

echo "Test empty bitset\n";
$it = new Teds\BitSet([]);
expect_throws(fn() => $it->offsetSet(0, true));
expect_throws(fn() => $it->set(0, strtoupper('value')));

echo "Test short bitset\n";
$it = new Teds\BitSet([true, false, true]);
$it->set(0, false);
$it->setBit(1, true);
$it->offsetSet(2, false);
echo json_encode($it), "\n";
expect_throws(fn() => $it->set(-1, true));
expect_throws(fn() => $it->set(3, true));
expect_throws(fn() => $it->set(3, false));
expect_throws(fn() => $it->set(0, 'invalid value'));
expect_throws(fn() => $it->set(PHP_INT_MAX, true));

?>
--EXPECT--
Test empty bitset
Caught OutOfBoundsException: Index out of range
Caught TypeError: Illegal Teds\BitSet value type string
Test short bitset
[false,true,false]
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught TypeError: Illegal Teds\BitSet value type string
Caught OutOfBoundsException: Index out of range