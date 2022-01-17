--TEST--
Teds\StrictSet cannot be re-initialized
--FILE--
<?php

$it = new Teds\StrictSet([]);

try {
    $it->__construct(['first']);
    echo "Unexpectedly called constructor\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);
try {
    $it->__unserialize([new ArrayObject(), new stdClass()]);
    echo "Unexpectedly called __unserialize\n";
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);
?>
--EXPECT--
Caught RuntimeException: Called Teds\StrictSet::__construct twice
object(Teds\StrictSet)#1 (0) {
}
Caught RuntimeException: Already unserialized
object(Teds\StrictSet)#1 (0) {
}