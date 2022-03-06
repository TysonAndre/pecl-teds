--TEST--
Teds\Set deduplicate
--FILE--
<?php
foreach ([Teds\StrictHashSet::class, Teds\StrictTreeSet::class, Teds\StrictSortedVectorSet::class] as $class_name) {
    echo "Test $class_name\n";
    $values = [];
    $set = new $class_name([1, 3, 5, 3, 3, 2, 1, 8]);
    printf("count=%d values=%s\n", count($set), json_encode($set->values()));
}
?>
--EXPECT--
Test Teds\StrictHashSet
count=5 values=[1,3,5,2,8]
Test Teds\StrictTreeSet
count=5 values=[1,2,3,5,8]
Test Teds\StrictSortedVectorSet
count=5 values=[1,2,3,5,8]