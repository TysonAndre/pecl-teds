--TEST--
Teds\IntVector unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $ser = 'O:14:"Teds\IntVector":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        var_dump(unserialize($ser));
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught RuntimeException: Teds\IntVector unserialize got invalid type tag, expected int