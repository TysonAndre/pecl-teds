--TEST--
Test Collections\clear() implementations
--FILE--
<?php

function test_empty_implementation(string $class_name) {
    echo "Testing $class_name\n";
    try {
        $collection = new $class_name([]);
        if ($collection->isEmpty() !== true) {
            throw new RuntimeException("FAIL - $class_name isEmpty implementation is wrong");
        }
        echo "values: ";
        var_dump($collection->values());
        echo "toArray: ";
        var_dump($collection->toArray());
        echo "clear: ";
        try {
            var_dump($collection->clear());
        } catch (Teds\UnsupportedOperationException $e) {
            echo "Caught {$e->getMessage()}\n";
        }
        var_dump($collection);
    } catch (Throwable $e) {
        echo "ERROR: Failed to test empty $class_name : {$e->getMessage()}\n";
    }
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
    test_empty_implementation($class_name);
}
?>
--EXPECT--
Testing Teds\BitSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\BitSet)#1 (0) {
}
Testing Teds\Deque
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\Deque)#1 (0) {
}
Testing Teds\CachedIterable
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\CachedIterable does not support clear - it is immutable
object(Teds\CachedIterable)#1 (0) {
}
Testing Teds\ImmutableIterable
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\ImmutableIterable does not support clear - it is immutable
object(Teds\ImmutableIterable)#2 (0) {
}
Testing Teds\ImmutableSequence
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\ImmutableSequence does not support clear - it is immutable
object(Teds\ImmutableSequence)#1 (0) {
}
Testing Teds\IntVector
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\IntVector)#2 (0) {
}
Testing Teds\LowMemoryVector
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\LowMemoryVector)#2 (0) {
}
Testing Teds\MutableIterable
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\MutableIterable)#2 (0) {
}
Testing Teds\StrictHashMap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictHashMap)#2 (0) {
}
Testing Teds\StrictHashSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictHashSet)#2 (0) {
}
Testing Teds\StrictMaxHeap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictMaxHeap)#2 (0) {
}
Testing Teds\StrictMinHeap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictMinHeap)#2 (0) {
}
Testing Teds\StrictSortedVectorMap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictSortedVectorMap)#2 (0) {
}
Testing Teds\StrictSortedVectorSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictSortedVectorSet)#2 (0) {
}
Testing Teds\StrictTreeMap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictTreeMap)#2 (0) {
}
Testing Teds\StrictTreeSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\StrictTreeSet)#2 (0) {
}
Testing Teds\Vector
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
object(Teds\Vector)#2 (0) {
}