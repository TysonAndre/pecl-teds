--TEST--
Teds\Collection constructed from large array
--FILE--
<?php
// discards keys
function test_collection_from_large_array(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(range(0, 299));
    var_dump($it->values() === range(0, 299));
    var_dump(count($it));
}

foreach ([
    Teds\CachedIterable::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\LowMemoryVector::class,
    Teds\IntVector::class,
    Teds\MutableIterable::class,
    Teds\StrictHashMap::class,
    Teds\StrictHashSet::class,
    Teds\SortedIntVectorSet::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictSortedVectorSet::class,
    Teds\StrictTreeMap::class,
    Teds\StrictTreeSet::class,
    Teds\Vector::class,
    Teds\Deque::class,
] as $class_name) {
    test_collection_from_large_array($class_name);
}

?>
--EXPECT--
Test Teds\CachedIterable
bool(true)
int(300)
Test Teds\ImmutableIterable
bool(true)
int(300)
Test Teds\ImmutableSequence
bool(true)
int(300)
Test Teds\LowMemoryVector
bool(true)
int(300)
Test Teds\IntVector
bool(true)
int(300)
Test Teds\MutableIterable
bool(true)
int(300)
Test Teds\StrictHashMap
bool(true)
int(300)
Test Teds\StrictHashSet
bool(true)
int(300)
Test Teds\SortedIntVectorSet
bool(true)
int(300)
Test Teds\StrictSortedVectorMap
bool(true)
int(300)
Test Teds\StrictSortedVectorSet
bool(true)
int(300)
Test Teds\StrictTreeMap
bool(true)
int(300)
Test Teds\StrictTreeSet
bool(true)
int(300)
Test Teds\Vector
bool(true)
int(300)
Test Teds\Deque
bool(true)
int(300)