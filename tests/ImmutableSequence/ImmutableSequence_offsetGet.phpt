--TEST--
Teds\ImmutableSequence offsetGet
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
expect_throws(fn() => (new ReflectionClass(Teds\ImmutableSequence::class))->newInstanceWithoutConstructor());
$it = new Teds\ImmutableSequence(['first' => new stdClass()]);
var_dump($it->offsetGet(0));
expect_throws(fn() => $it->offsetSet(0,'x'));
expect_throws(fn() => $it->offsetUnset(0));
var_dump($it->offsetGet('0'));
echo "offsetExists checks\n";
var_dump($it->offsetExists(1));
var_dump($it->offsetExists('1'));
var_dump($it->offsetExists(PHP_INT_MAX));
var_dump($it->offsetExists(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet(PHP_INT_MAX));
expect_throws(fn() => $it->offsetGet(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->offsetGet(PHP_INT_MAX));
expect_throws(fn() => $it->offsetGet(PHP_INT_MIN));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->offsetGet(-1));
expect_throws(fn() => $it->offsetGet(1));
expect_throws(fn() => $it->offsetGet('1'));
expect_throws(fn() => $it->offsetGet('invalid'));
expect_throws(fn() => $it[['invalid']]);
expect_throws(fn() => $it->offsetUnset(PHP_INT_MAX));
expect_throws(fn() => $it->offsetSet(PHP_INT_MAX,'x'));
expect_throws(function () use ($it) { unset($it[0]); });
var_dump($it->getIterator());
?>
--EXPECT--
Caught ReflectionException: Class Teds\ImmutableSequence is an internal class marked as final that cannot be instantiated without invoking its constructor
object(stdClass)#1 (0) {
}
Caught RuntimeException: ImmutableSequence does not support offsetSet - it is immutable
Caught RuntimeException: ImmutableSequence does not support offsetUnset - it is immutable
object(stdClass)#1 (0) {
}
offsetExists checks
bool(false)
bool(false)
bool(false)
bool(false)
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: Index invalid or out of range
Caught RuntimeException: ImmutableSequence does not support offsetUnset - it is immutable
Caught RuntimeException: ImmutableSequence does not support offsetSet - it is immutable
Caught RuntimeException: ImmutableSequence does not support offsetUnset - it is immutable
object(InternalIterator)#4 (0) {
}