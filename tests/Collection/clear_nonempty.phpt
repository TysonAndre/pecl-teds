--TEST--
Test Collections\clear() non-empty refcounted
--FILE--
<?php

function test_empty_implementation(string $class_name) {
    echo "Testing $class_name\n";
    try {
        $collection = new $class_name(["v_$class_name"]);
        if ($collection->isEmpty() !== false) {
            throw new RuntimeException("FAIL - $class_name isEmpty implementation is wrong");
        }
        if ($collection->count() !== 1) {
            throw new RuntimeException("FAIL - $class_name count implementation is wrong");
        }
        var_dump($collection);
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
        echo "ERROR: Failed to test non-empty $class_name : {$e->getMessage()}\n";
    }
    echo "\n";
}

foreach ([
    // Teds\BitSet::class,
    Teds\Deque::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    // Teds\IntVector::class,
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
    Teds\CachedIterable::class,
] as $class_name) {
    test_empty_implementation($class_name);
}
?>
--EXPECT--
Testing Teds\Deque
object(Teds\Deque)#1 (1) {
  [0]=>
  string(12) "v_Teds\Deque"
}
values: array(1) {
  [0]=>
  string(12) "v_Teds\Deque"
}
toArray: array(1) {
  [0]=>
  string(12) "v_Teds\Deque"
}
clear: NULL
object(Teds\Deque)#1 (0) {
}

Testing Teds\ImmutableIterable
object(Teds\ImmutableIterable)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(24) "v_Teds\ImmutableIterable"
  }
}
values: array(1) {
  [0]=>
  string(24) "v_Teds\ImmutableIterable"
}
toArray: array(1) {
  [0]=>
  string(24) "v_Teds\ImmutableIterable"
}
clear: Caught Teds\ImmutableIterable does not support clear - it is immutable
object(Teds\ImmutableIterable)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(24) "v_Teds\ImmutableIterable"
  }
}

Testing Teds\ImmutableSequence
object(Teds\ImmutableSequence)#2 (1) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
}
values: array(1) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
}
toArray: array(1) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
}
clear: Caught Teds\ImmutableSequence does not support clear - it is immutable
object(Teds\ImmutableSequence)#2 (1) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
}

Testing Teds\LowMemoryVector
object(Teds\LowMemoryVector)#1 (1) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
}
values: array(1) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
}
toArray: array(1) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
}
clear: NULL
object(Teds\LowMemoryVector)#1 (0) {
}

Testing Teds\MutableIterable
object(Teds\MutableIterable)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(22) "v_Teds\MutableIterable"
  }
}
values: array(1) {
  [0]=>
  string(22) "v_Teds\MutableIterable"
}
toArray: array(1) {
  [0]=>
  string(22) "v_Teds\MutableIterable"
}
clear: NULL
object(Teds\MutableIterable)#1 (0) {
}

Testing Teds\StrictHashMap
object(Teds\StrictHashMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(20) "v_Teds\StrictHashMap"
  }
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictHashMap"
}
toArray: array(1) {
  [0]=>
  string(20) "v_Teds\StrictHashMap"
}
clear: NULL
object(Teds\StrictHashMap)#1 (0) {
}

Testing Teds\StrictHashSet
object(Teds\StrictHashSet)#1 (1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
toArray: array(1) {
  ["v_Teds\StrictHashSet"]=>
  string(20) "v_Teds\StrictHashSet"
}
clear: NULL
object(Teds\StrictHashSet)#1 (0) {
}

Testing Teds\StrictMaxHeap
object(Teds\StrictMaxHeap)#1 (0) {
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
}
toArray: array(1) {
  ["v_Teds\StrictMaxHeap"]=>
  string(20) "v_Teds\StrictMaxHeap"
}
clear: NULL
object(Teds\StrictMaxHeap)#1 (0) {
}

Testing Teds\StrictMinHeap
object(Teds\StrictMinHeap)#1 (1) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
}
toArray: array(1) {
  ["v_Teds\StrictMinHeap"]=>
  string(20) "v_Teds\StrictMinHeap"
}
clear: NULL
object(Teds\StrictMinHeap)#1 (0) {
}

Testing Teds\StrictSortedVectorMap
object(Teds\StrictSortedVectorMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(28) "v_Teds\StrictSortedVectorMap"
  }
}
values: array(1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorMap"
}
toArray: array(1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorMap"
}
clear: NULL
object(Teds\StrictSortedVectorMap)#1 (0) {
}

Testing Teds\StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#1 (1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
}
values: array(1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
}
toArray: array(1) {
  ["v_Teds\StrictSortedVectorSet"]=>
  string(28) "v_Teds\StrictSortedVectorSet"
}
clear: NULL
object(Teds\StrictSortedVectorSet)#1 (0) {
}

Testing Teds\StrictTreeMap
object(Teds\StrictTreeMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(20) "v_Teds\StrictTreeMap"
  }
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictTreeMap"
}
toArray: array(1) {
  [0]=>
  string(20) "v_Teds\StrictTreeMap"
}
clear: NULL
object(Teds\StrictTreeMap)#1 (0) {
}

Testing Teds\StrictTreeSet
object(Teds\StrictTreeSet)#1 (1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
toArray: array(1) {
  ["v_Teds\StrictTreeSet"]=>
  string(20) "v_Teds\StrictTreeSet"
}
clear: NULL
object(Teds\StrictTreeSet)#1 (0) {
}

Testing Teds\Vector
object(Teds\Vector)#1 (1) {
  [0]=>
  string(13) "v_Teds\Vector"
}
values: array(1) {
  [0]=>
  string(13) "v_Teds\Vector"
}
toArray: array(1) {
  [0]=>
  string(13) "v_Teds\Vector"
}
clear: NULL
object(Teds\Vector)#1 (0) {
}

Testing Teds\CachedIterable
object(Teds\CachedIterable)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(21) "v_Teds\CachedIterable"
  }
}
values: array(1) {
  [0]=>
  string(21) "v_Teds\CachedIterable"
}
toArray: array(1) {
  [0]=>
  string(21) "v_Teds\CachedIterable"
}
clear: Caught Teds\CachedIterable does not support clear - it is immutable
object(Teds\CachedIterable)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(21) "v_Teds\CachedIterable"
  }
}