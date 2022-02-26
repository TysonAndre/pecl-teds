--TEST--
Test empty Collections with ArrayAccess
--FILE--
<?php

function test_empty_implementation(string $class_name) {
    echo "Testing $class_name\n";
    try {
        $collection = new $class_name((function () { yield from []; })());
        if ($collection instanceof Teds\Map) {
            echo "skip caught set test - $class_name is a Map\n";
        } else {
            try {
                $collection[0] = 123;
            } catch (Throwable $t) {
                printf("set %s: %s\n", get_class($t), $t->getMessage());
            }
            try {
                $collection->offsetSet(0, 123);
            } catch (Throwable $t) {
                printf("offsetSet %s: %s\n", get_class($t), $t->getMessage());
            }
        }
        try {
            var_dump($collection[0]);
        } catch (Throwable $t) {
            printf("get caught %s: %s\n", get_class($t), $t->getMessage());
        }
        try {
            var_dump($collection->offsetGet(0));
        } catch (Throwable $t) {
            printf("offsetGet caught %s: %s\n", get_class($t), $t->getMessage());
        }
        if (isset($collection[0])) {
            throw new RuntimeException("Fail - isset 0 should return false for empty Collection");
        }
        if (!empty($collection[0])) {
            throw new RuntimeException("Fail - empty 0 should return true for empty Collection");
        }
        try {
            unset($collection[1]);
            echo "Successfully unset offset 1 on empty collection\n";
        } catch (Throwable $t) {
            printf("unset caught %s: %s\n", get_class($t), $t->getMessage());
        }
        var_dump($collection);
    } catch (Throwable $e) {
        echo "ERROR: Failed to test non-empty $class_name : {$e->getMessage()}\n";
    }
    echo "\n";
}

foreach ([
    Teds\BitVector::class,
    Teds\Deque::class,
    Teds\IntVector::class,
    Teds\StrictHashMap::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictTreeMap::class,
    Teds\Vector::class,
] as $class_name) {
    test_empty_implementation($class_name);
    echo "\n";
}
?>
--EXPECTF--
Testing Teds\BitVector
set TypeError: Illegal Teds\BitVector value type int
offsetSet TypeError: Illegal Teds\BitVector value type int
get caught OutOfBoundsException: Index out of range
offsetGet caught OutOfBoundsException: Index out of range
unset caught OutOfBoundsException: Index out of range
object(Teds\BitVector)#%d (0) {
}


Testing Teds\Deque
set RuntimeException: Index invalid or out of range
offsetSet OutOfBoundsException: Index out of range
get caught OutOfBoundsException: Index out of range
offsetGet caught OutOfBoundsException: Index out of range
unset caught OutOfBoundsException: Index out of range
object(Teds\Deque)#%d (0) {
}


Testing Teds\IntVector
set OutOfBoundsException: Index out of range
offsetSet OutOfBoundsException: Index out of range
get caught OutOfBoundsException: Index out of range
offsetGet caught OutOfBoundsException: Index out of range
unset caught OutOfBoundsException: Index out of range
object(Teds\IntVector)#%d (0) {
}


Testing Teds\StrictHashMap
skip caught set test - Teds\StrictHashMap is a Map
get caught OutOfBoundsException: Key not found
offsetGet caught OutOfBoundsException: Key not found
Successfully unset offset 1 on empty collection
object(Teds\StrictHashMap)#%d (0) {
}


Testing Teds\StrictSortedVectorMap
skip caught set test - Teds\StrictSortedVectorMap is a Map
get caught OutOfBoundsException: Key not found
offsetGet caught OutOfBoundsException: Key not found
Successfully unset offset 1 on empty collection
object(Teds\StrictSortedVectorMap)#%d (0) {
}


Testing Teds\StrictTreeMap
skip caught set test - Teds\StrictTreeMap is a Map
get caught OutOfBoundsException: Key not found
offsetGet caught OutOfBoundsException: Key not found
Successfully unset offset 1 on empty collection
object(Teds\StrictTreeMap)#%d (0) {
}


Testing Teds\Vector
set OutOfBoundsException: Index out of range
offsetSet OutOfBoundsException: Index out of range
get caught OutOfBoundsException: Index out of range
offsetGet caught OutOfBoundsException: Index out of range
unset caught OutOfBoundsException: Index out of range
object(Teds\Vector)#%d (0) {
}