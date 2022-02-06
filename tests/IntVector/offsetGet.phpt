--TEST--
Teds\IntVector offsetGet/get
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
expect_throws(fn() => (new ReflectionClass(Teds\IntVector::class))->newInstanceWithoutConstructor());
$it = new Teds\IntVector([12345]);
var_dump($it->offsetGet(0));
var_dump($it->get(0));
expect_throws(fn() => $it->offsetSet(1,12345));
expect_throws(fn() => $it->offsetUnset(0));
var_dump($it->offsetGet('0'));
echo "offsetExists checks\n";
var_dump($it->offsetExists(1));
var_dump($it->offsetExists('1'));
var_dump($it->offsetExists(PHP_INT_MAX));
var_dump($it->offsetExists(PHP_INT_MIN));
expect_throws(fn() => $it->get(1));
expect_throws(fn() => $it->get(-1));
echo "Invalid offsetGet calls\n";
expect_throws(fn() => $it->offsetGet(PHP_INT_MAX));
expect_throws(fn() => $it->offsetGet(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->get(PHP_INT_MAX));
expect_throws(fn() => $it->get(PHP_INT_MIN));
expect_throws(fn() => $it->get(1));
expect_throws(fn() => $it->get(-1));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->offsetGet('1'));
expect_throws(fn() => $it->offsetGet('invalid'));
expect_throws(fn() => $it->get('invalid'));
expect_throws(fn() => $it[['invalid']]);
expect_throws(fn() => $it->offsetUnset(PHP_INT_MAX));
expect_throws(fn() => $it->offsetSet(PHP_INT_MAX,'x'));
expect_throws(function () use ($it) { unset($it[0]); });
var_dump($it->getIterator());
?>
--EXPECT--
Caught ReflectionException: Class Teds\IntVector is an internal class marked as final that cannot be instantiated without invoking its constructor
int(12345)
int(12345)
Caught OutOfBoundsException: Index out of range
Caught RuntimeException: Teds\IntVector does not support offsetUnset - elements must be removed by resizing
int(12345)
offsetExists checks
bool(false)
bool(false)
bool(false)
bool(false)
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Invalid offsetGet calls
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught TypeError: Illegal offset type string
Caught TypeError: Teds\IntVector::get(): Argument #1 ($offset) must be of type int, string given
Caught TypeError: Illegal offset type array
Caught RuntimeException: Teds\IntVector does not support offsetUnset - elements must be removed by resizing
Caught TypeError: Illegal Teds\IntVector value type string
Caught RuntimeException: Teds\IntVector does not support offsetUnset - elements must be removed by resizing
object(InternalIterator)#1 (0) {
}