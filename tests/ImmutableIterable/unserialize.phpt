--TEST--
Teds\ImmutableIterable unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\ImmutableIterable(['first' => 'second']);
    $ser = 'O:22:"Teds\ImmutableIterable":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    $ser = 'O:22:"Teds\ImmutableIterable":1:{i:0;s:5:"first";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\ImmutableIterable::__unserialize saw unexpected string key, expected sequence of keys and values
Caught UnexpectedValueException: Odd number of elements
