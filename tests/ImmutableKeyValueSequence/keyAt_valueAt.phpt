--TEST--
Teds\ImmutableKeyValueSequence keyAt and valueAt()
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
expect_throws(fn() => (new ReflectionClass(Teds\ImmutableKeyValueSequence::class))->newInstanceWithoutConstructor());
$emptyIt = new Teds\ImmutableKeyValueSequence([]);
expect_throws(fn() => $emptyIt->keyAt(0));
expect_throws(fn() => $emptyIt->keyAt(PHP_INT_MAX));
expect_throws(fn() => $emptyIt->keyAt(-1));
$it = new Teds\ImmutableKeyValueSequence(['first' => new stdClass()]);
var_dump($it->keyAt(0));
var_dump($it->valueAt(0));
expect_throws(fn() => $it->keyAt(PHP_INT_MAX));
expect_throws(fn() => $it->keyAt(PHP_INT_MIN));
expect_throws(fn() => $it->keyAt(1));
expect_throws(fn() => $it->keyAt(-1));
expect_throws(fn() => $it->valueAt(1));
expect_throws(fn() => $it->valueAt(-1));
?>
--EXPECT--
Caught ReflectionException: Class Teds\ImmutableKeyValueSequence is an internal class marked as final that cannot be instantiated without invoking its constructor
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
string(5) "first"
object(stdClass)#1 (0) {
}
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
