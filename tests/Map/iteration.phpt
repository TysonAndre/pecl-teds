--TEST--
Teds\Map iteration and modification
--FILE--
<?php

$create_key = fn ($i) => "k_$i";
// TODO Change StrictTreeMap iteration.
// TODO Make StrictHashMap account for repacking the hash set (not enough additions and removals in this test to trigger it).
foreach ([Teds\StrictSortedVectorMap::class, Teds\StrictHashMap::class, Teds\StrictTreeMap::class] as $class_name) {
    echo "Test $class_name\n";
    $map = new $class_name([$create_key(0) => new stdClass(), $create_key(10) => $create_key(100), $create_key(20) => $create_key(200), $create_key(30) => false]);
    foreach ($map as $key => $value) {
        printf("key: %s, value: %s\n", json_encode($key), json_encode($value));
        if ($key === "k_0") {
            $map->offsetUnset("k_0");
            unset($map["k_10"]);
        } elseif ($key === "k_20") {
            $map["k_25"] = $create_key("123");
        } elseif ($key === "k_30") {
            $map["k_35"] = new stdClass();
        }
    }
    printf("count=%d\n", count($map));
    echo json_encode($map), "\n";
}
?>
--EXPECT--
Test Teds\StrictSortedVectorMap
key: "k_0", value: {}
key: "k_20", value: "k_200"
key: "k_25", value: "k_123"
key: "k_30", value: false
key: "k_35", value: {}
count=4
[["k_20","k_200"],["k_25","k_123"],["k_30",false],["k_35",{}]]
Test Teds\StrictHashMap
key: "k_0", value: {}
key: "k_20", value: "k_200"
key: "k_30", value: false
key: "k_25", value: "k_123"
key: "k_35", value: {}
count=4
[["k_20","k_200"],["k_30",false],["k_25","k_123"],["k_35",{}]]
Test Teds\StrictTreeMap
key: "k_0", value: {}
count=2
[["k_20","k_200"],["k_30",false]]