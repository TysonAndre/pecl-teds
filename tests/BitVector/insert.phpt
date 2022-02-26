--TEST--
Teds\BitVector insert
--FILE--
<?php
foreach ([Teds\BitVector::class, Teds\Vector::class, Teds\Deque::class, Teds\LowMemoryVector::class] as $class) {
    echo "Test $class\n";
    $bs = new $class();
    $bs->insert(0, true, false, true, true);
    echo json_encode($bs), "\n";
    $bs->insert(0);
    $bs->insert(2, false, true, false);
    echo json_encode($bs), "\n";
    $bs->insert(7, false, true, true, true, false, true);
    try {
        $bs->insert(PHP_INT_MIN, true);
    } catch (Exception $e) {
        printf("Threw %s: %s\n", $e::class, $e->getMessage());
    }
    try {
        $bs->insert(count($bs) + 1, true);
    } catch (Exception $e) {
        printf("Threw %s: %s\n", $e::class, $e->getMessage());
    }
    echo json_encode($bs), "\n";
}
--EXPECT--
Test Teds\BitVector
[true,false,true,true]
[true,false,false,true,false,true,true]
Threw OutOfBoundsException: Index out of range
Threw OutOfBoundsException: Index out of range
[true,false,false,true,false,true,true,false,true,true,true,false,true]
Test Teds\Vector
[true,false,true,true]
[true,false,false,true,false,true,true]
Threw OutOfBoundsException: Index out of range
Threw OutOfBoundsException: Index out of range
[true,false,false,true,false,true,true,false,true,true,true,false,true]
Test Teds\Deque
[true,false,true,true]
[true,false,false,true,false,true,true]
Threw OutOfBoundsException: Index out of range
Threw OutOfBoundsException: Index out of range
[true,false,false,true,false,true,true,false,true,true,true,false,true]
Test Teds\LowMemoryVector
[true,false,true,true]
[true,false,false,true,false,true,true]
Threw OutOfBoundsException: Index out of range
Threw OutOfBoundsException: Index out of range
[true,false,false,true,false,true,true,false,true,true,true,false,true]