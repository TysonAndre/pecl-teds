--TEST--
Teds\Set iteration and modification
--FILE--
<?php

$create_key = fn ($i) => "k_$i";
// TODO Change StrictTreeSet iteration.
// TODO Make StrictHashSet account for repacking the hash set (not enough additions and removals in this test to trigger it).
foreach ([Teds\StrictSortedVectorSet::class, Teds\StrictHashSet::class, Teds\StrictTreeSet::class] as $class_name) {
    echo "Test $class_name\n";
    $set = new $class_name([$create_key(0), $create_key(10), $create_key(20), $create_key(30)]);
    foreach ($set as $value) {
        var_dump($value);
        if ($value === "k_0") {
            $set->remove("k_0");
            $set->remove("k_10");
        } elseif ($value === "k_20") {
            $set->add("k_25");
        } elseif ($value === "k_30") {
            $set->add("k_35");
        }
    }
    echo json_encode($set), "\n";
}
?>
--EXPECT--
Test Teds\StrictSortedVectorSet
string(3) "k_0"
string(4) "k_20"
string(4) "k_25"
string(4) "k_30"
string(4) "k_35"
["k_20","k_25","k_30","k_35"]
Test Teds\StrictHashSet
string(3) "k_0"
string(4) "k_20"
string(4) "k_30"
string(4) "k_25"
string(4) "k_35"
["k_20","k_30","k_25","k_35"]
Test Teds\StrictTreeSet
string(3) "k_0"
["k_20","k_30"]