--TEST--
Teds\ImmutableIterable keyAt and valueAt()
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
foreach ([Teds\ImmutableIterable::class, Teds\CachedIterable::class] as $class_name) {
    echo "Test $class_name\n";
    expect_throws(fn() => (new ReflectionClass($class_name))->newInstanceWithoutConstructor());
    $emptyIt = new $class_name([]);
    expect_throws(fn() => $emptyIt->keyAt(0));
    expect_throws(fn() => $emptyIt->keyAt(PHP_INT_MAX));
    expect_throws(fn() => $emptyIt->keyAt(-1));
    $it = new $class_name(['first' => new stdClass()]);
    var_dump($it->keyAt(0));
    var_dump($it->valueAt(0));
    expect_throws(fn() => $it->keyAt(PHP_INT_MAX));
    expect_throws(fn() => $it->keyAt(PHP_INT_MIN));
    expect_throws(fn() => $it->keyAt(1));
    expect_throws(fn() => $it->keyAt(-1));
    expect_throws(fn() => $it->valueAt(1));
    expect_throws(fn() => $it->valueAt(-1));
}
?>
--EXPECT--
Test Teds\ImmutableIterable
Caught ReflectionException: Class Teds\ImmutableIterable is an internal class marked as final that cannot be instantiated without invoking its constructor
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
Test Teds\CachedIterable
Caught ReflectionException: Class Teds\CachedIterable is an internal class marked as final that cannot be instantiated without invoking its constructor
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
string(5) "first"
object(stdClass)#3 (0) {
}
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range