--TEST--
Teds\Set iteration with gaps
--FILE--
<?php

$create_key = fn ($i) => "k_$i";
foreach ([Teds\StrictSortedVectorSet::class, Teds\StrictTreeSet::class, Teds\StrictHashSet::class] as $class_name) {
    echo "Test $class_name\n";
    $set = new $class_name([$create_key(0), $create_key(10), $create_key(20), $create_key(30), $create_key(40)]);
    $it = $set->getIterator();
    foreach ($it as $value) {
        var_dump($value);
        if ($value === "k_0") {
            $set->remove($value);
            $set->remove("k_10");
        } elseif ($value === "k_20") {
            $set->add("k_25");
        } elseif ($value === "k_40") {
            $set->add("k_50");
            $set->add("k_35");
            $set->remove("k_50");
        }
        if (!$it->valid()) {
            echo "Iterator::valid() == false\n";  // When next is called the iterator becomes valid again.
        }
    }
    echo json_encode($set), "\n";
}
?>
--EXPECT--
Test Teds\StrictSortedVectorSet
string(3) "k_0"
Iterator::valid() == false
string(4) "k_20"
string(4) "k_25"
string(4) "k_30"
string(4) "k_40"
["k_20","k_25","k_30","k_35","k_40"]
Test Teds\StrictTreeSet
string(3) "k_0"
Iterator::valid() == false
string(4) "k_20"
string(4) "k_25"
string(4) "k_30"
string(4) "k_40"
["k_20","k_25","k_30","k_35","k_40"]
Test Teds\StrictHashSet
string(3) "k_0"
Iterator::valid() == false
string(4) "k_20"
string(4) "k_30"
string(4) "k_40"
string(4) "k_25"
string(4) "k_35"
["k_20","k_30","k_40","k_25","k_35"]
