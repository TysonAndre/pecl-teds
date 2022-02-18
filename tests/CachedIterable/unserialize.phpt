--TEST--
Teds\CachedIterable unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\CachedIterable(['first' => 'second']);
    $ser = 'O:19:"Teds\CachedIterable":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    $ser = 'O:19:"Teds\CachedIterable":1:{i:0;s:5:"first";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\CachedIterable::__unserialize saw unexpected string key, expected sequence of keys and values
Caught UnexpectedValueException: Odd number of elements
