--TEST--
Teds\BitVector cannot be re-initialized
--FILE--
<?php

$it = new Teds\BitVector([]);

try {
    $it->__construct([true]);
    echo "Unexpectedly called constructor\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);
try {
    $it->__unserialize([false, new stdClass()]);
    echo "Unexpectedly called __unserialize\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);
?>
--EXPECT--
Caught RuntimeException: Called Teds\BitVector::__construct twice
object(Teds\BitVector)#1 (0) {
}
Caught RuntimeException: Teds\BitVector already unserialized
object(Teds\BitVector)#1 (0) {
}