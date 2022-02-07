--TEST--
Teds\IntVector offsetSet/set
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

echo "Test empty vector\n";
$it = new Teds\IntVector([]);
expect_throws(fn() => $it->offsetSet(0, -123456));
expect_throws(fn() => $it->set(0, strtoupper('value')));

echo "Test short vector\n";
$it = new Teds\IntVector([1, 2, 3]);
$it->set(0, 123);
$it->offsetSet(2, 123);
echo json_encode($it), "\n";
expect_throws(fn() => $it->set(-1, 123));
expect_throws(fn() => $it->set(3, 151));
expect_throws(fn() => $it->set(0, 'invalid value'));
expect_throws(fn() => $it->set(PHP_INT_MAX, 123));

?>
--EXPECT--
Test empty vector
Caught OutOfBoundsException: Index out of range
Caught TypeError: Teds\IntVector::set(): Argument #2 ($value) must be of type int, string given
Test short vector
[123,2,123]
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught TypeError: Teds\IntVector::set(): Argument #2 ($value) must be of type int, string given
Caught OutOfBoundsException: Index out of range