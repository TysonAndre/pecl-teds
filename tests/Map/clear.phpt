--TEST--
Teds\Map clear
--FILE--
<?php

function test_clear_map(string $class_name) {
    $it = new $class_name();
    for ($i = 0; $i < 11; $i++) {
        $it["k$i"] = "v$i";
    }
    echo json_encode($it), "\n";
    $it->clear();
    echo "After clear\n";
    var_dump($it);
}

foreach ([
    Teds\StrictHashMap::class,
    Teds\StrictTreeMap::class,
    Teds\StrictSortedVectorMap::class,
] as $class_name) {
    test_clear_map($class_name);
}
?>
--EXPECT--
[["k0","v0"],["k1","v1"],["k2","v2"],["k3","v3"],["k4","v4"],["k5","v5"],["k6","v6"],["k7","v7"],["k8","v8"],["k9","v9"],["k10","v10"]]
After clear
object(Teds\StrictHashMap)#1 (0) {
}
[["k0","v0"],["k1","v1"],["k10","v10"],["k2","v2"],["k3","v3"],["k4","v4"],["k5","v5"],["k6","v6"],["k7","v7"],["k8","v8"],["k9","v9"]]
After clear
object(Teds\StrictTreeMap)#1 (0) {
}
[["k0","v0"],["k1","v1"],["k10","v10"],["k2","v2"],["k3","v3"],["k4","v4"],["k5","v5"],["k6","v6"],["k7","v7"],["k8","v8"],["k9","v9"]]
After clear
object(Teds\StrictSortedVectorMap)#1 (0) {
}