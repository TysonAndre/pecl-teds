--TEST--
Teds\Sequence offsetSet/set
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

foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
] as $class_name) {
    echo "Test empty $class_name\n";
    $it = new $class_name([]);
    expect_throws(fn() => $it->offsetSet(0, strtoupper('value')));
    expect_throws(fn() => $it->set(0, strtoupper('value')));

    $str = "Test short $class_name";
    $it = new $class_name(explode(' ', $str));
    $it->set(0, 'new');
    $it->offsetSet(2, strtoupper('test'));
    echo json_encode($it), "\n";
    expect_throws(fn() => $it->set(-1, strtoupper('value')));
    expect_throws(fn() => $it->set(3, 'end'));
    expect_throws(fn() => $it->set(PHP_INT_MAX, 'end'));
}

?>
--EXPECT--
Test empty Teds\Deque
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
["new","short","TEST"]
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Test empty Teds\Vector
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
["new","short","TEST"]
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Test empty Teds\LowMemoryVector
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
["new","short","TEST"]
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range