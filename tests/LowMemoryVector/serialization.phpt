--TEST--
Teds\LowMemoryVector can be serialized and unserialized
--FILE--
<?php

$it = new Teds\LowMemoryVector([new stdClass()]);
try {
    $it->dynamicProp = 123;
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
/*
$ser = serialize($it);
echo $ser, "\n";
foreach (unserialize($ser) as $key => $value) {
    echo "Entry:\n";
    var_dump($key, $value);
}
var_dump($ser === serialize($it));
echo "Done\n";
$x = 123;
$it = new Teds\LowMemoryVector([]);
var_dump($it->__serialize());
 */
?>
--EXPECT--
Caught Error: Cannot create dynamic property Teds\LowMemoryVector::$dynamicProp