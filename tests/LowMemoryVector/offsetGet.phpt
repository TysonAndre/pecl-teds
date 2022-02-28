--TEST--
Teds\LowMemoryVector offsetGet/get
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
expect_throws(fn() => (new ReflectionClass(Teds\LowMemoryVector::class))->newInstanceWithoutConstructor());
$it = new Teds\LowMemoryVector([new stdClass()]);
var_dump($it->offsetGet(0));
var_dump($it->get(0));
expect_throws(fn() => $it->offsetSet(1,'x'));
expect_throws(fn() => $it->offsetUnset(-1));
expect_throws(fn() => $it->offsetUnset(count($it)));
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
// @phan-suppress-next-line PhanTypeMismatchArgumentInternalReal
expect_throws(fn() => $it->get('invalid'));
expect_throws(fn() => $it[['invalid']]);
expect_throws(fn() => $it->offsetUnset(PHP_INT_MAX));
expect_throws(fn() => $it->offsetUnset(PHP_INT_MIN));
expect_throws(fn() => $it->offsetSet(PHP_INT_MAX,'x'));
expect_throws(function () use ($it) { unset($it[-1]); });
var_dump($it->getIterator());
var_dump($it);
unset($it[0]);
var_dump($it);
?>
--EXPECTF--
Caught ReflectionException: Class Teds\LowMemoryVector is an internal class marked as final that cannot be instantiated without invoking its constructor
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
object(stdClass)#%d (0) {
}
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
Caught TypeError: Teds\LowMemoryVector::get(): Argument #1 ($offset) must be of type int, string given
Caught TypeError: Illegal offset type array
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
object(InternalIterator)#%d (0) {
}
object(Teds\LowMemoryVector)#%d (0) {
}
object(Teds\LowMemoryVector)#%d (0) {
}