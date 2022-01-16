--TEST--
Teds\SortedStrictMap unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\SortedStrictMap(['first' => 'second']);
    $ser = 'O:20:"Teds\SortedStrictMap":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    $ser = 'O:20:"Teds\SortedStrictMap":1:{i:0;s:5:"first";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\SortedStrictMap::__unserialize saw unexpected string key, expected sequence of keys and values
Caught UnexpectedValueException: Odd number of elements
