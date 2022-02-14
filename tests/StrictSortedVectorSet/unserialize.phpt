--TEST--
Teds\StrictSortedVectorSet unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\StrictSortedVectorSet(['first' => 'second']);
    $ser = 'O:26:"Teds\StrictSortedVectorSet":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\StrictSortedVectorSet::__unserialize saw unexpected string key, expected sequence of values
