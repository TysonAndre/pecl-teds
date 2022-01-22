--TEST--
Teds\StableSortedListSet can be serialized and unserialized
--FILE--
<?php

$it = new Teds\StableSortedListSet(['first' => new stdClass()]);
try {
    $it->dynamicProp = 123;
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
$ser = serialize($it);
echo $ser, "\n";
foreach (unserialize($ser) as $value) {
    echo "Item: ";
    var_dump($value);
}
var_dump($ser === serialize($it));
echo "Done\n";
$x = 123;
$it = new Teds\StableSortedListSet([]);
var_dump($it->__serialize());
?>
--EXPECT--
Caught Error: Cannot create dynamic property Teds\StableSortedListSet::$dynamicProp
O:24:"Teds\StableSortedListSet":1:{i:0;O:8:"stdClass":0:{}}
Item: object(stdClass)#5 (0) {
}
bool(true)
Done
array(0) {
}