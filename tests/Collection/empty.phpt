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
        if (stripos($class_name, 'Heap') === false) {
            try {
                var_dump($collection->first());
            } catch (Throwable $e) {
                printf("first() Caught %s: %s\n", $e::class, $e->getMessage());
            }
            try {
                var_dump($collection->last());
            } catch (Throwable $e) {
                printf("last() Caught %s: %s\n", $e::class, $e->getMessage());
            }
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
    Teds\ImmutableSortedStringSet::class,
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
    test_empty_implementation($class_name);
}
?>
--EXPECTF--
Testing Teds\BitSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first bit of empty Teds\BitSet
last() Caught UnderflowException: Cannot read last bit of empty Teds\BitSet
object(Teds\BitSet)#1 (0) {
}
Testing Teds\Deque
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\Deque
last() Caught UnderflowException: Cannot read last value of empty Teds\Deque
object(Teds\Deque)#3 (0) {
}
Testing Teds\CachedIterable
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\CachedIterable does not support clear - it is immutable
first() Caught Error: Call to undefined method Teds\CachedIterable::first()
last() Caught Error: Call to undefined method Teds\CachedIterable::last()
object(Teds\CachedIterable)#2 (0) {
}
Testing Teds\ImmutableIterable
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\ImmutableIterable does not support clear - it is immutable
first() Caught Error: Call to undefined method Teds\ImmutableIterable::first()
last() Caught Error: Call to undefined method Teds\ImmutableIterable::last()
object(Teds\ImmutableIterable)#3 (0) {
}
Testing Teds\ImmutableSequence
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\ImmutableSequence does not support clear - it is immutable
first() Caught UnderflowException: Cannot get first element of empty Teds\ImmutableSequence
last() Caught UnderflowException: Cannot get last element of empty Teds\ImmutableSequence
object(Teds\ImmutableSequence)#2 (0) {
}
Testing Teds\ImmutableSortedStringSet
values: array(0) {
}
toArray: array(0) {
}
clear: Caught Teds\ImmutableSortedStringSet is immutable
first() Caught UnderflowException: Cannot read first value of empty Teds\ImmutableSortedStringSet
last() Caught UnderflowException: Cannot read last value of empty Teds\ImmutableSortedStringSet
object(Teds\ImmutableSortedStringSet)#3 (0) {
}
Testing Teds\IntVector
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\IntVector
last() Caught UnderflowException: Cannot read last value of empty Teds\IntVector
object(Teds\IntVector)#2 (0) {
}
Testing Teds\SortedIntVectorSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\IntVector
last() Caught UnderflowException: Cannot read last value of empty Teds\IntVector
object(Teds\SortedIntVectorSet)#1 (0) {
}
Testing Teds\LowMemoryVector
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\LowMemoryVector
last() Caught UnderflowException: Cannot read last value of empty Teds\LowMemoryVector
object(Teds\LowMemoryVector)#3 (0) {
}
Testing Teds\MutableIterable
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught Error: Call to undefined method Teds\MutableIterable::first()
last() Caught Error: Call to undefined method Teds\MutableIterable::last()
object(Teds\MutableIterable)#2 (0) {
}
Testing Teds\StrictHashMap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught Error: Call to undefined method Teds\StrictHashMap::first()
last() Caught Error: Call to undefined method Teds\StrictHashMap::last()
object(Teds\StrictHashMap)#1 (0) {
}
Testing Teds\StrictHashSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught Error: Call to undefined method Teds\StrictHashSet::first()
last() Caught Error: Call to undefined method Teds\StrictHashSet::last()
object(Teds\StrictHashSet)#3 (0) {
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
first() Caught UnderflowException: Cannot read first of empty StrictSortedVectorMap
last() Caught UnderflowException: Cannot read last of empty StrictSortedVectorMap
object(Teds\StrictSortedVectorMap)#2 (0) {
}
Testing Teds\StrictSortedVectorSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first of empty StrictSortedVectorSet
last() Caught UnderflowException: Cannot read last of empty StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#1 (0) {
}
Testing Teds\StrictTreeMap
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first of empty StrictTreeMap
last() Caught UnderflowException: Cannot read last of empty StrictTreeMap
object(Teds\StrictTreeMap)#3 (0) {
}
Testing Teds\StrictTreeSet
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty StrictTreeSet
last() Caught UnderflowException: Cannot read last value of empty StrictTreeSet
object(Teds\StrictTreeSet)#2 (0) {
}
Testing Teds\Vector
values: array(0) {
}
toArray: array(0) {
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\Vector
last() Caught UnderflowException: Cannot read last value of empty Teds\Vector
object(Teds\Vector)#1 (0) {
}