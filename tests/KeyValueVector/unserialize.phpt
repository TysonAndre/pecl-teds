--TEST--
Teds\KeyValueVector unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\KeyValueVector(['first' => 'second']);
    $ser = 'O:19:"Teds\KeyValueVector":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    $ser = 'O:19:"Teds\KeyValueVector":1:{i:0;s:5:"first";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\KeyValueVector::__unserialize saw unexpected string key, expected sequence of keys and values
Caught UnexpectedValueException: Odd number of elements
