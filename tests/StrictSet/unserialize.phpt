--TEST--
Teds\StrictSet unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $it = new Teds\StrictSet(['first' => 'second']);
    $ser = 'O:14:"Teds\StrictSet":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        unserialize($ser);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught UnexpectedValueException: Teds\StrictSet::__unserialize saw unexpected string key, expected sequence of values
