--TEST--
Teds\ImmutableSequence array access
--FILE--
<?php

function expect_throws(Closure $cb): void {
    try {
        $cb();
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
        return;
    }
    throw new RuntimeException("Unexpectedly didn't throw");
}
call_user_func(function () {
    $it = new Teds\ImmutableSequence(['first' => new stdClass(), 'second' => null, 'third' => [new stdClass()]]);
    var_dump($it->offsetGet(0));
    var_dump($it->get(0));
    expect_throws(fn() => $it->offsetSet(0,new stdClass()));
    expect_throws(fn() => $it->offsetUnset(0));
    expect_throws(fn() => $it[0] ??= 'other');
    expect_throws(fn() => $it[1] ??= 'other');
    expect_throws(function () use ($it) { unset($it[1]); });
    echo "Test array modification\n";
    expect_throws(function () use ($it) { $it[2][0] = 123; });
    var_dump($it);
});

?>
--EXPECT--
object(stdClass)#3 (0) {
}
object(stdClass)#3 (0) {
}
Caught Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support offsetSet - it is immutable
Caught Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support offsetUnset - it is immutable
Caught Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support modification - it is immutable
Caught Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support modification - it is immutable
Caught Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support offsetUnset - it is immutable
Test array modification
Caught Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support offsetSet - it is immutable
object(Teds\ImmutableSequence)#2 (3) {
  [0]=>
  object(stdClass)#3 (0) {
  }
  [1]=>
  NULL
  [2]=>
  array(1) {
    [0]=>
    object(stdClass)#4 (0) {
    }
  }
}