--TEST--
Teds\Map iteration with repeated add and remove
--FILE--
<?php

$create_key = fn ($i) => "k_$i";
// TODO Change StrictTreeMap iteration.
// TODO Make StrictHashMap account for repacking the hash set (not enough additions and removals in this test to trigger it).
foreach ([Teds\StrictSortedVectorMap::class, Teds\StrictHashMap::class, Teds\StrictTreeMap::class] as $class_name) {
    echo "Test $class_name\n";
    $set = new $class_name([$create_key(10) => "v10", $create_key(11) => "v11"]);
    $i = 12;
    foreach ($set as $key => $value) {
        echo "Saw $key => $value\n";
        $set->offsetUnset($key);
        $set->offsetSet($create_key($i), "v$i");
        $i++;
        if ($i >= 30) {
            break;
        }
    }
    echo json_encode($set), "\n";
}
?>
--EXPECT--
Test Teds\StrictSortedVectorMap
Saw k_10 => v10
Saw k_11 => v11
Saw k_12 => v12
Saw k_13 => v13
Saw k_14 => v14
Saw k_15 => v15
Saw k_16 => v16
Saw k_17 => v17
Saw k_18 => v18
Saw k_19 => v19
Saw k_20 => v20
Saw k_21 => v21
Saw k_22 => v22
Saw k_23 => v23
Saw k_24 => v24
Saw k_25 => v25
Saw k_26 => v26
Saw k_27 => v27
[["k_28","v28"],["k_29","v29"]]
Test Teds\StrictHashMap
Saw k_10 => v10
Saw k_11 => v11
Saw k_12 => v12
Saw k_13 => v13
Saw k_14 => v14
Saw k_15 => v15
Saw k_16 => v16
Saw k_17 => v17
Saw k_18 => v18
Saw k_19 => v19
Saw k_20 => v20
Saw k_21 => v21
Saw k_22 => v22
Saw k_23 => v23
Saw k_24 => v24
Saw k_25 => v25
Saw k_26 => v26
Saw k_27 => v27
[["k_28","v28"],["k_29","v29"]]
Test Teds\StrictTreeMap
Saw k_10 => v10
Saw k_11 => v11
Saw k_12 => v12
Saw k_13 => v13
Saw k_14 => v14
Saw k_15 => v15
Saw k_16 => v16
Saw k_17 => v17
Saw k_18 => v18
Saw k_19 => v19
Saw k_20 => v20
Saw k_21 => v21
Saw k_22 => v22
Saw k_23 => v23
Saw k_24 => v24
Saw k_25 => v25
Saw k_26 => v26
Saw k_27 => v27
[["k_28","v28"],["k_29","v29"]]