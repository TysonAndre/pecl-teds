--TEST--
Teds\Collection cannot be re-initialized
--FILE--
<?php

function test_no_reinitialize(string $class_name) {
    $it = new $class_name([]);

    try {
        $it->__construct(['first']);
        echo "Unexpectedly called constructor\n";
    } catch (Throwable $t) {
        printf("Caught %s: %s\n", $t::class, $t->getMessage());
    }
    try {
        $it->__unserialize([new $class_name([]), new stdClass()]);
        echo "Unexpectedly called __unserialize\n";
    } catch (Throwable $t) {
        printf("Caught %s: %s\n", $t::class, $t->getMessage());
    }
    var_dump($it);
}
foreach ([
    Teds\BitSet::class,
    Teds\Deque::class,
    Teds\CachedIterable::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\IntVector::class,
    Teds\SortedIntVectorSet::class,
    Teds\LowMemoryVector::class,
    Teds\MutableIterable::class,
    Teds\StrictHashMap::class,
    Teds\StrictHashSet::class,
    Teds\StrictMaxHeap::class,
    Teds\StrictMinHeap::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictSortedVectorSet::class,
    Teds\StrictTreeMap::class,
    Teds\StrictTreeSet::class,
    Teds\Vector::class,
] as $class_name) {
    test_no_reinitialize($class_name);
}
?>
--EXPECTF--
Caught RuntimeException: Called Teds\BitSet::__construct twice
Caught RuntimeException: Teds\BitSet already unserialized
object(Teds\BitSet)#%d (0) {
}
Caught RuntimeException: Called Teds\Deque::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\Deque)#%d (0) {
}
Caught RuntimeException: Called Teds\CachedIterable::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\CachedIterable)#%d (0) {
}
Caught RuntimeException: Called Teds\ImmutableIterable::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\ImmutableIterable)#%d (0) {
}
Caught RuntimeException: Called Teds\ImmutableSequence::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\ImmutableSequence)#%d (0) {
}
Caught RuntimeException: Called Teds\IntVector::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\IntVector)#%d (0) {
}
Caught RuntimeException: Called Teds\SortedIntVectorSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\SortedIntVectorSet)#%d (0) {
}
Caught RuntimeException: Called Teds\LowMemoryVector::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\LowMemoryVector)#%d (0) {
}
Caught RuntimeException: Called Teds\MutableIterable::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\MutableIterable)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictHashMap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictHashMap)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictHashSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictHashSet)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictHeap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictMaxHeap)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictHeap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictMinHeap)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictSortedVectorMap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictSortedVectorMap)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictSortedVectorSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictSortedVectorSet)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictTreeMap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictTreeMap)#%d (0) {
}
Caught RuntimeException: Called Teds\StrictTreeSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictTreeSet)#%d (0) {
}
Caught RuntimeException: Called Teds\Vector::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\Vector)#%d (0) {
}