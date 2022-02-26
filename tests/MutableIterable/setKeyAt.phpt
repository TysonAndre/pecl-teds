--TEST--
Teds\MutableIterable keyAt and valueAt()
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
expect_throws(fn() => (new ReflectionClass(Teds\MutableIterable::class))->newInstanceWithoutConstructor());
$emptyIt = new Teds\MutableIterable([]);
expect_throws(fn() => $emptyIt->keyAt(0));
expect_throws(fn() => $emptyIt->setKeyAt(0, new stdClass()));
expect_throws(fn() => $emptyIt->setValueAt(0, new stdClass()));
$it = new Teds\MutableIterable(['first' => new stdClass()]);
$it->setKeyAt(0, new ArrayObject());
$it->setKeyAt(0, new ArrayObject());
var_dump($it->keyAt(0));
var_dump($it->valueAt(0));
?>
--EXPECTF--
Caught ReflectionException: Class Teds\MutableIterable is an internal class marked as final that cannot be instantiated without invoking its constructor
Caught OutOfBoundsException: Offset out of range
Caught OutOfBoundsException: Offset out of range
Caught OutOfBoundsException: Offset out of range
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(stdClass)#%d (0) {
}