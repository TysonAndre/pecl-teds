--TEST--
Teds\BitSet cannot be re-initialized
--FILE--
<?php

$it = new Teds\BitSet([]);

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
Caught RuntimeException: Called Teds\BitSet::__construct twice
object(Teds\BitSet)#1 (0) {
}
Caught RuntimeException: Teds\BitSet already unserialized
object(Teds\BitSet)#1 (0) {
}