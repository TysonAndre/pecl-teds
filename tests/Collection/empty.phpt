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
        debug_zval_dump($collection);
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
        echo "array_walk: ";
        var_dump(array_walk($collection, function ($value, $key) { echo "Not called\n"; }));
        echo "get_mangled_object_vars: ";
        echo json_encode(get_mangled_object_vars($collection)), "\n";
        echo "reset: ";
        ini_set('error_reporting', E_ALL & ~E_DEPRECATED);
        var_dump(reset($collection)); // reset is deprecated
        ini_set('error_reporting', E_ALL);
    } catch (Throwable $e) {
        echo "ERROR: Failed to test empty $class_name : {$e->getMessage()}\n";
    }
}

foreach ([
    Teds\BitVector::class,
    Teds\Deque::class,
    Teds\CachedIterable::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\ImmutableSortedIntSet::class,
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
Testing Teds\BitVector
values: array(0) {
}
toArray: array(0) {
}
object(Teds\BitVector)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first bit of empty Teds\BitVector
last() Caught UnderflowException: Cannot read last bit of empty Teds\BitVector
object(Teds\BitVector)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\Deque
values: array(0) {
}
toArray: array(0) {
}
object(Teds\Deque)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\Deque
last() Caught UnderflowException: Cannot read last value of empty Teds\Deque
object(Teds\Deque)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\CachedIterable
values: array(0) {
}
toArray: array(0) {
}
object(Teds\CachedIterable)#%d (0) refcount(2){
}
clear: Caught Teds\CachedIterable does not support clear - it is immutable
first() Caught Error: Call to undefined method Teds\CachedIterable::first()
last() Caught Error: Call to undefined method Teds\CachedIterable::last()
object(Teds\CachedIterable)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\ImmutableIterable
values: array(0) {
}
toArray: array(0) {
}
object(Teds\ImmutableIterable)#%d (0) refcount(2){
}
clear: Caught Teds\ImmutableIterable does not support clear - it is immutable
first() Caught Error: Call to undefined method Teds\ImmutableIterable::first()
last() Caught Error: Call to undefined method Teds\ImmutableIterable::last()
object(Teds\ImmutableIterable)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\ImmutableSequence
values: array(0) {
}
toArray: array(0) {
}
object(Teds\ImmutableSequence)#%d (0) refcount(2){
}
clear: Caught Teds\ImmutableSequence is immutable
first() Caught UnderflowException: Cannot get first element of empty Teds\ImmutableSequence
last() Caught UnderflowException: Cannot get last element of empty Teds\ImmutableSequence
object(Teds\ImmutableSequence)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\ImmutableSortedIntSet
values: array(0) {
}
toArray: array(0) {
}
object(Teds\ImmutableSortedIntSet)#%d (0) refcount(2){
}
clear: Caught Teds\ImmutableSortedIntSet is immutable
first() Caught UnderflowException: Cannot read first value of empty Teds\IntVector
last() Caught UnderflowException: Cannot read last value of empty Teds\IntVector
object(Teds\ImmutableSortedIntSet)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\ImmutableSortedStringSet
values: array(0) {
}
toArray: array(0) {
}
object(Teds\ImmutableSortedStringSet)#%d (0) refcount(2){
}
clear: Caught Teds\ImmutableSortedStringSet is immutable
first() Caught UnderflowException: Cannot read first value of empty Teds\ImmutableSortedStringSet
last() Caught UnderflowException: Cannot read last value of empty Teds\ImmutableSortedStringSet
object(Teds\ImmutableSortedStringSet)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\IntVector
values: array(0) {
}
toArray: array(0) {
}
object(Teds\IntVector)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\IntVector
last() Caught UnderflowException: Cannot read last value of empty Teds\IntVector
object(Teds\IntVector)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\SortedIntVectorSet
values: array(0) {
}
toArray: array(0) {
}
object(Teds\SortedIntVectorSet)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\IntVector
last() Caught UnderflowException: Cannot read last value of empty Teds\IntVector
object(Teds\SortedIntVectorSet)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\LowMemoryVector
values: array(0) {
}
toArray: array(0) {
}
object(Teds\LowMemoryVector)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\LowMemoryVector
last() Caught UnderflowException: Cannot read last value of empty Teds\LowMemoryVector
object(Teds\LowMemoryVector)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\MutableIterable
values: array(0) {
}
toArray: array(0) {
}
object(Teds\MutableIterable)#%d (0) refcount(2){
}
clear: NULL
first() Caught Error: Call to undefined method Teds\MutableIterable::first()
last() Caught Error: Call to undefined method Teds\MutableIterable::last()
object(Teds\MutableIterable)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictHashMap
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictHashMap)#%d (0) refcount(2){
}
clear: NULL
first() Caught Error: Call to undefined method Teds\StrictHashMap::first()
last() Caught Error: Call to undefined method Teds\StrictHashMap::last()
object(Teds\StrictHashMap)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictHashSet
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictHashSet)#%d (0) refcount(2){
}
clear: NULL
first() Caught Error: Call to undefined method Teds\StrictHashSet::first()
last() Caught Error: Call to undefined method Teds\StrictHashSet::last()
object(Teds\StrictHashSet)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictMaxHeap
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictMaxHeap)#%d (0) refcount(2){
}
clear: NULL
object(Teds\StrictMaxHeap)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictMinHeap
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictMinHeap)#%d (0) refcount(2){
}
clear: NULL
object(Teds\StrictMinHeap)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictSortedVectorMap
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictSortedVectorMap)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first of empty Teds\StrictSortedVectorMap
last() Caught UnderflowException: Cannot read last of empty Teds\StrictSortedVectorMap
object(Teds\StrictSortedVectorMap)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictSortedVectorSet
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictSortedVectorSet)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\StrictSortedVectorSet
last() Caught UnderflowException: Cannot read last value of empty Teds\StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictTreeMap
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictTreeMap)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first of empty StrictTreeMap
last() Caught UnderflowException: Cannot read last of empty StrictTreeMap
object(Teds\StrictTreeMap)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\StrictTreeSet
values: array(0) {
}
toArray: array(0) {
}
object(Teds\StrictTreeSet)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty StrictTreeSet
last() Caught UnderflowException: Cannot read last value of empty StrictTreeSet
object(Teds\StrictTreeSet)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)
Testing Teds\Vector
values: array(0) {
}
toArray: array(0) {
}
object(Teds\Vector)#%d (0) refcount(2){
}
clear: NULL
first() Caught UnderflowException: Cannot read first value of empty Teds\Vector
last() Caught UnderflowException: Cannot read last value of empty Teds\Vector
object(Teds\Vector)#%d (0) {
}
array_walk: bool(true)
get_mangled_object_vars: []
reset: bool(false)