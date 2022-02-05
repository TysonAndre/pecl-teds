--TEST--
Teds\LowMemoryVector unserialize error handling
--FILE--
<?php

call_user_func(function () {
    $ser = 'O:20:"Teds\LowMemoryVector":2:{i:0;s:5:"first";s:5:"unexp";s:6:"second";}';
    try {
        var_dump(unserialize($ser));
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
});
?>
--EXPECT--
Caught RuntimeException: LowMemoryVector __unserialize not yet implemented