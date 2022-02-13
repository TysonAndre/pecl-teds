--TEST--
Teds\StrictTreeSet unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\StrictTreeSet(['first' => 'second']);
    $ser = 'O:18:"Teds\StrictTreeSet":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\StrictTreeSet::__unserialize saw unexpected string key, expected sequence of values
