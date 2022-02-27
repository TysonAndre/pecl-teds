--TEST--
Test Collections\clear() non-empty refcounted
--FILE--
<?php

function test_nonempty_implementation(string $class_name) {
    echo "Testing $class_name\n";
    try {
        $collection = new $class_name(["v_$class_name", "z_$class_name"]);
        if ($collection->isEmpty() !== false) {
            throw new RuntimeException("FAIL - $class_name isEmpty implementation is wrong");
        }
        if ($collection->count() !== 2) {
            throw new RuntimeException("FAIL - $class_name count implementation is wrong");
        }
        var_dump($collection);
        echo "values: ";
        var_dump($collection->values());
        echo "toArray: ";
        var_dump($collection->toArray());
        if (method_exists($collection, 'last')) {
            echo "First: ";
            var_dump($collection->first());
            echo "Last: ";
            var_dump($collection->last());
        }

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
    // Teds\BitVector::class,
    Teds\Deque::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\ImmutableSortedStringSet::class,
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
    test_nonempty_implementation($class_name);
}
?>
--EXPECTF--
Testing Teds\Deque
object(Teds\Deque)#%d (2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "z_Teds\Deque"
}
values: array(2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "z_Teds\Deque"
}
toArray: array(2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "z_Teds\Deque"
}
First: string(12) "v_Teds\Deque"
Last: string(12) "z_Teds\Deque"
clear: NULL
object(Teds\Deque)#%d (0) {
}

Testing Teds\ImmutableIterable
object(Teds\ImmutableIterable)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(24) "v_Teds\ImmutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(24) "z_Teds\ImmutableIterable"
  }
}
values: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableIterable"
  [1]=>
  string(24) "z_Teds\ImmutableIterable"
}
toArray: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableIterable"
  [1]=>
  string(24) "z_Teds\ImmutableIterable"
}
clear: Caught Teds\ImmutableIterable does not support clear - it is immutable
object(Teds\ImmutableIterable)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(24) "v_Teds\ImmutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(24) "z_Teds\ImmutableIterable"
  }
}

Testing Teds\ImmutableSequence
object(Teds\ImmutableSequence)#%d (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "z_Teds\ImmutableSequence"
}
values: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "z_Teds\ImmutableSequence"
}
toArray: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "z_Teds\ImmutableSequence"
}
First: string(24) "v_Teds\ImmutableSequence"
Last: string(24) "z_Teds\ImmutableSequence"
clear: Caught Teds\ImmutableSequence is immutable
object(Teds\ImmutableSequence)#%d (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "z_Teds\ImmutableSequence"
}

Testing Teds\ImmutableSortedStringSet
object(Teds\ImmutableSortedStringSet)#%d (2) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
  [1]=>
  string(31) "z_Teds\ImmutableSortedStringSet"
}
values: array(2) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
  [1]=>
  string(31) "z_Teds\ImmutableSortedStringSet"
}
toArray: array(2) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
  [1]=>
  string(31) "z_Teds\ImmutableSortedStringSet"
}
First: string(31) "v_Teds\ImmutableSortedStringSet"
Last: string(31) "z_Teds\ImmutableSortedStringSet"
clear: Caught Teds\ImmutableSortedStringSet is immutable
object(Teds\ImmutableSortedStringSet)#%d (2) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
  [1]=>
  string(31) "z_Teds\ImmutableSortedStringSet"
}

Testing Teds\LowMemoryVector
object(Teds\LowMemoryVector)#%d (2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  string(22) "z_Teds\LowMemoryVector"
}
values: array(2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  string(22) "z_Teds\LowMemoryVector"
}
toArray: array(2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  string(22) "z_Teds\LowMemoryVector"
}
First: string(22) "v_Teds\LowMemoryVector"
Last: string(22) "z_Teds\LowMemoryVector"
clear: NULL
object(Teds\LowMemoryVector)#%d (0) {
}

Testing Teds\MutableIterable
object(Teds\MutableIterable)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(22) "v_Teds\MutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(22) "z_Teds\MutableIterable"
  }
}
values: array(2) {
  [0]=>
  string(22) "v_Teds\MutableIterable"
  [1]=>
  string(22) "z_Teds\MutableIterable"
}
toArray: array(2) {
  [0]=>
  string(22) "v_Teds\MutableIterable"
  [1]=>
  string(22) "z_Teds\MutableIterable"
}
clear: NULL
object(Teds\MutableIterable)#%d (0) {
}

Testing Teds\StrictHashMap
object(Teds\StrictHashMap)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(20) "v_Teds\StrictHashMap"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(20) "z_Teds\StrictHashMap"
  }
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictHashMap"
  [1]=>
  string(20) "z_Teds\StrictHashMap"
}
toArray: array(2) {
  [0]=>
  string(20) "v_Teds\StrictHashMap"
  [1]=>
  string(20) "z_Teds\StrictHashMap"
}
clear: NULL
object(Teds\StrictHashMap)#%d (0) {
}

Testing Teds\StrictHashSet
object(Teds\StrictHashSet)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
  [1]=>
  string(20) "z_Teds\StrictHashSet"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
  [1]=>
  string(20) "z_Teds\StrictHashSet"
}
toArray: array(2) {
  ["v_Teds\StrictHashSet"]=>
  string(20) "v_Teds\StrictHashSet"
  ["z_Teds\StrictHashSet"]=>
  string(20) "z_Teds\StrictHashSet"
}
clear: NULL
object(Teds\StrictHashSet)#%d (0) {
}

Testing Teds\StrictMaxHeap
object(Teds\StrictMaxHeap)#%d (2) {
  [0]=>
  string(20) "z_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
values: array(2) {
  [0]=>
  string(20) "z_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
toArray: array(2) {
  ["z_Teds\StrictMaxHeap"]=>
  string(20) "z_Teds\StrictMaxHeap"
  ["v_Teds\StrictMaxHeap"]=>
  string(20) "v_Teds\StrictMaxHeap"
}
clear: NULL
object(Teds\StrictMaxHeap)#%d (0) {
}

Testing Teds\StrictMinHeap
object(Teds\StrictMinHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "z_Teds\StrictMinHeap"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "z_Teds\StrictMinHeap"
}
toArray: array(2) {
  ["v_Teds\StrictMinHeap"]=>
  string(20) "v_Teds\StrictMinHeap"
  ["z_Teds\StrictMinHeap"]=>
  string(20) "z_Teds\StrictMinHeap"
}
clear: NULL
object(Teds\StrictMinHeap)#%d (0) {
}

Testing Teds\StrictSortedVectorMap
object(Teds\StrictSortedVectorMap)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(28) "v_Teds\StrictSortedVectorMap"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(28) "z_Teds\StrictSortedVectorMap"
  }
}
values: array(2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorMap"
  [1]=>
  string(28) "z_Teds\StrictSortedVectorMap"
}
toArray: array(2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorMap"
  [1]=>
  string(28) "z_Teds\StrictSortedVectorMap"
}
First: string(28) "v_Teds\StrictSortedVectorMap"
Last: string(28) "z_Teds\StrictSortedVectorMap"
clear: NULL
object(Teds\StrictSortedVectorMap)#%d (0) {
}

Testing Teds\StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#%d (2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
  [1]=>
  string(28) "z_Teds\StrictSortedVectorSet"
}
values: array(2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
  [1]=>
  string(28) "z_Teds\StrictSortedVectorSet"
}
toArray: array(2) {
  ["v_Teds\StrictSortedVectorSet"]=>
  string(28) "v_Teds\StrictSortedVectorSet"
  ["z_Teds\StrictSortedVectorSet"]=>
  string(28) "z_Teds\StrictSortedVectorSet"
}
First: string(28) "v_Teds\StrictSortedVectorSet"
Last: string(28) "z_Teds\StrictSortedVectorSet"
clear: NULL
object(Teds\StrictSortedVectorSet)#%d (0) {
}

Testing Teds\StrictTreeMap
object(Teds\StrictTreeMap)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(20) "v_Teds\StrictTreeMap"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(20) "z_Teds\StrictTreeMap"
  }
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictTreeMap"
  [1]=>
  string(20) "z_Teds\StrictTreeMap"
}
toArray: array(2) {
  [0]=>
  string(20) "v_Teds\StrictTreeMap"
  [1]=>
  string(20) "z_Teds\StrictTreeMap"
}
First: string(20) "v_Teds\StrictTreeMap"
Last: string(20) "z_Teds\StrictTreeMap"
clear: NULL
object(Teds\StrictTreeMap)#%d (0) {
}

Testing Teds\StrictTreeSet
object(Teds\StrictTreeSet)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
  [1]=>
  string(20) "z_Teds\StrictTreeSet"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
  [1]=>
  string(20) "z_Teds\StrictTreeSet"
}
toArray: array(2) {
  ["v_Teds\StrictTreeSet"]=>
  string(20) "v_Teds\StrictTreeSet"
  ["z_Teds\StrictTreeSet"]=>
  string(20) "z_Teds\StrictTreeSet"
}
First: string(20) "v_Teds\StrictTreeSet"
Last: string(20) "z_Teds\StrictTreeSet"
clear: NULL
object(Teds\StrictTreeSet)#%d (0) {
}

Testing Teds\Vector
object(Teds\Vector)#%d (2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "z_Teds\Vector"
}
values: array(2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "z_Teds\Vector"
}
toArray: array(2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "z_Teds\Vector"
}
First: string(13) "v_Teds\Vector"
Last: string(13) "z_Teds\Vector"
clear: NULL
object(Teds\Vector)#%d (0) {
}

Testing Teds\CachedIterable
object(Teds\CachedIterable)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(21) "v_Teds\CachedIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(21) "z_Teds\CachedIterable"
  }
}
values: array(2) {
  [0]=>
  string(21) "v_Teds\CachedIterable"
  [1]=>
  string(21) "z_Teds\CachedIterable"
}
toArray: array(2) {
  [0]=>
  string(21) "v_Teds\CachedIterable"
  [1]=>
  string(21) "z_Teds\CachedIterable"
}
clear: Caught Teds\CachedIterable does not support clear - it is immutable
object(Teds\CachedIterable)#%d (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(21) "v_Teds\CachedIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(21) "z_Teds\CachedIterable"
  }
}