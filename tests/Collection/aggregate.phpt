--TEST--
Teds\Collection IteratorAggregate implementations
--FILE--
<?php

function test_immutableaggregate(string $class_name) {
    echo "Test $class_name\n";
    $it = new Teds\ImmutableIterable(['first' => 'x', 'second' => (object)['key' => 'value']]);
    foreach ($it as $k1 => $v1) {
        foreach ($it as $k2 => $v2) {
            printf("k1=%s k2=%s v1=%s v2=%s\n", json_encode($k1), json_encode($k2), json_encode($v1), json_encode($v2));
        }
    }
}
foreach ([
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\LowMemoryVector::class,
    Teds\MutableIterable::class,
    Teds\StrictHashMap::class,
    Teds\StrictHashSet::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictSortedVectorSet::class,
    Teds\StrictTreeMap::class,
    Teds\StrictTreeSet::class,
    Teds\Vector::class,
    Teds\Deque::class,
] as $class_name) {
    test_immutableaggregate($class_name);
}

?>
--EXPECT--
Test Teds\ImmutableIterable
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\ImmutableSequence
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\LowMemoryVector
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\MutableIterable
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\StrictHashMap
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\StrictHashSet
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\StrictSortedVectorMap
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\StrictSortedVectorSet
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\StrictTreeMap
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\StrictTreeSet
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\Vector
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}
Test Teds\Deque
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}