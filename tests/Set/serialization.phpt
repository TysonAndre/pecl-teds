--TEST--
Teds\Set can be serialized and unserialized
--FILE--
<?php

function test_serialize(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(['first' => new stdClass()]);
    $ser = serialize($it);
    echo $ser, "\n";
    foreach (unserialize($ser) as $value) {
        echo "Item: ";
        var_dump($value);
    }
    var_dump($ser === serialize($it));
    echo "Done\n";
    $x = 123;
    $it = new Teds\StrictHashSet([]);
    var_dump($it->__serialize());
}
foreach ([
    Teds\StrictTreeMap::class,
    Teds\StrictHashMap::class,
    Teds\StrictSortedVectorMap::class,
] as $class_name) {
    test_serialize($class_name);
}
?>
--EXPECT--
Test Teds\StrictTreeMap
O:18:"Teds\StrictTreeMap":2:{i:0;s:5:"first";i:1;O:8:"stdClass":0:{}}
Item: object(stdClass)#4 (0) {
}
bool(true)
Done
array(0) {
}
Test Teds\StrictHashMap
O:18:"Teds\StrictHashMap":2:{i:0;s:5:"first";i:1;O:8:"stdClass":0:{}}
Item: object(stdClass)#2 (0) {
}
bool(true)
Done
array(0) {
}
Test Teds\StrictSortedVectorMap
O:26:"Teds\StrictSortedVectorMap":2:{i:0;s:5:"first";i:1;O:8:"stdClass":0:{}}
Item: object(stdClass)#5 (0) {
}
bool(true)
Done
array(0) {
}