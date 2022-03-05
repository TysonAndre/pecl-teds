--TEST--
Teds\Set iteration with repeated add and remove
--FILE--
<?php

$create_key = fn ($i) => "k_$i";
// TODO Change StrictTreeSet iteration.
// TODO Make StrictHashSet account for repacking the hash set (not enough additions and removals in this test to trigger it).
foreach ([Teds\StrictSortedVectorSet::class, Teds\StrictHashSet::class, Teds\StrictTreeSet::class] as $class_name) {
    echo "Test $class_name\n";
    $set = new $class_name([$create_key(10), $create_key(11)]);
    $i = 12;
    foreach ($set as $value) {
        echo "Saw $value\n";
        $set->remove($value);
        $set->add($create_key($i++));
        if ($i >= 30) {
            break;
        }
    }
    echo json_encode($set), "\n";
}
?>
--EXPECT--
Test Teds\StrictSortedVectorSet
Saw k_10
Saw k_11
Saw k_12
Saw k_13
Saw k_14
Saw k_15
Saw k_16
Saw k_17
Saw k_18
Saw k_19
Saw k_20
Saw k_21
Saw k_22
Saw k_23
Saw k_24
Saw k_25
Saw k_26
Saw k_27
["k_28","k_29"]
Test Teds\StrictHashSet
Saw k_10
Saw k_11
Saw k_12
Saw k_13
Saw k_14
Saw k_15
Saw k_16
Saw k_17
Saw k_18
Saw k_19
Saw k_20
Saw k_21
Saw k_22
Saw k_23
Saw k_24
Saw k_25
Saw k_26
Saw k_27
["k_28","k_29"]
Test Teds\StrictTreeSet
Saw k_10
Saw k_11
Saw k_12
Saw k_13
Saw k_14
Saw k_15
Saw k_16
Saw k_17
Saw k_18
Saw k_19
Saw k_20
Saw k_21
Saw k_22
Saw k_23
Saw k_24
Saw k_25
Saw k_26
Saw k_27
["k_28","k_29"]