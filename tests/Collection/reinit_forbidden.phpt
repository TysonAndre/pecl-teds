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
--EXPECT--
Caught RuntimeException: Called Teds\BitSet::__construct twice
Caught RuntimeException: Teds\BitSet already unserialized
object(Teds\BitSet)#1 (0) {
}
Caught RuntimeException: Called Teds\Deque::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\Deque)#5 (0) {
}
Caught RuntimeException: Called Teds\CachedIterable::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\CachedIterable)#2 (0) {
}
Caught RuntimeException: Called Teds\ImmutableIterable::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\ImmutableIterable)#4 (0) {
}
Caught RuntimeException: Called Teds\ImmutableSequence::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\ImmutableSequence)#1 (0) {
}
Caught RuntimeException: Called Teds\IntVector::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\IntVector)#5 (0) {
}
Caught RuntimeException: Called Teds\LowMemoryVector::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\LowMemoryVector)#2 (0) {
}
Caught RuntimeException: Called Teds\MutableIterable::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\MutableIterable)#4 (0) {
}
Caught RuntimeException: Called Teds\StrictHashMap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictHashMap)#1 (0) {
}
Caught RuntimeException: Called Teds\StrictHashSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictHashSet)#5 (0) {
}
Caught RuntimeException: Called Teds\StrictHeap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictMaxHeap)#2 (0) {
}
Caught RuntimeException: Called Teds\StrictHeap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictMinHeap)#4 (0) {
}
Caught RuntimeException: Called Teds\StrictSortedVectorMap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictSortedVectorMap)#1 (0) {
}
Caught RuntimeException: Called Teds\StrictSortedVectorSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictSortedVectorSet)#5 (0) {
}
Caught RuntimeException: Called Teds\StrictTreeMap::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictTreeMap)#2 (0) {
}
Caught RuntimeException: Called Teds\StrictTreeSet::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\StrictTreeSet)#4 (0) {
}
Caught RuntimeException: Called Teds\Vector::__construct twice
Caught RuntimeException: Already unserialized
object(Teds\Vector)#1 (0) {
}