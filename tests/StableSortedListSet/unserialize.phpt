--TEST--
Teds\StableSortedListSet unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\StableSortedListSet(['first' => 'second']);
    $ser = 'O:24:"Teds\StableSortedListSet":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\StableSortedListSet::__unserialize saw unexpected string key, expected sequence of values
