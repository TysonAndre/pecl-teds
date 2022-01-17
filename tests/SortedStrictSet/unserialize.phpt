--TEST--
Teds\SortedStrictSet unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\SortedStrictSet(['first' => 'second']);
    $ser = 'O:20:"Teds\SortedStrictSet":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\SortedStrictSet::__unserialize saw unexpected string key, expected sequence of values
