--TEST--
Teds\Collections create from array with references
--FILE--
<?php

function test_nonempty_implementation(string $class_name) {
    echo "Testing $class_name\n";
    try {
        $s = "v_$class_name";
        $collection = new $class_name([&$s, &$s]);
        var_dump($collection);
        printf("count=%d\n", count($collection));
        var_dump($collection);
        echo "values: ";
        var_dump($collection->values());
    } catch (Throwable $e) {
        echo "ERROR: Failed to test non-empty $class_name : {$e->getMessage()}\n";
    }
    echo "Testing $class_name from Traversable\n";
    try {
        $s = "v_$class_name";
        $ao = new ArrayObject([&$s, &$s]);

        $collection = new $class_name($ao);
        var_dump($collection);
        printf("count=%d\n", count($collection));
        var_dump($collection);
        echo "values: ";
        var_dump($collection->values());
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
  string(12) "v_Teds\Deque"
}
count=2
object(Teds\Deque)#%d (2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "v_Teds\Deque"
}
values: array(2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "v_Teds\Deque"
}
Testing Teds\Deque from Traversable
object(Teds\Deque)#%d (2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "v_Teds\Deque"
}
count=2
object(Teds\Deque)#%d (2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "v_Teds\Deque"
}
values: array(2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  string(12) "v_Teds\Deque"
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
    string(24) "v_Teds\ImmutableIterable"
  }
}
count=2
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
    string(24) "v_Teds\ImmutableIterable"
  }
}
values: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableIterable"
  [1]=>
  string(24) "v_Teds\ImmutableIterable"
}
Testing Teds\ImmutableIterable from Traversable
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
    string(24) "v_Teds\ImmutableIterable"
  }
}
count=2
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
    string(24) "v_Teds\ImmutableIterable"
  }
}
values: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableIterable"
  [1]=>
  string(24) "v_Teds\ImmutableIterable"
}

Testing Teds\ImmutableSequence
object(Teds\ImmutableSequence)#%d (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "v_Teds\ImmutableSequence"
}
count=2
object(Teds\ImmutableSequence)#%d (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "v_Teds\ImmutableSequence"
}
values: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "v_Teds\ImmutableSequence"
}
Testing Teds\ImmutableSequence from Traversable
object(Teds\ImmutableSequence)#%d (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "v_Teds\ImmutableSequence"
}
count=2
object(Teds\ImmutableSequence)#%d (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "v_Teds\ImmutableSequence"
}
values: array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  string(24) "v_Teds\ImmutableSequence"
}

Testing Teds\ImmutableSortedStringSet
object(Teds\ImmutableSortedStringSet)#%d (1) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
}
count=1
object(Teds\ImmutableSortedStringSet)#%d (1) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
}
values: array(1) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
}
Testing Teds\ImmutableSortedStringSet from Traversable
object(Teds\ImmutableSortedStringSet)#%d (1) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
}
count=1
object(Teds\ImmutableSortedStringSet)#%d (1) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
}
values: array(1) {
  [0]=>
  string(31) "v_Teds\ImmutableSortedStringSet"
}

Testing Teds\LowMemoryVector
object(Teds\LowMemoryVector)#%d (0) {
}
count=2
object(Teds\LowMemoryVector)#%d (0) {
}
values: array(2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  string(22) "v_Teds\LowMemoryVector"
}
Testing Teds\LowMemoryVector from Traversable
object(Teds\LowMemoryVector)#%d (0) {
}
count=2
object(Teds\LowMemoryVector)#%d (0) {
}
values: array(2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  string(22) "v_Teds\LowMemoryVector"
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
    string(22) "v_Teds\MutableIterable"
  }
}
count=2
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
    string(22) "v_Teds\MutableIterable"
  }
}
values: array(2) {
  [0]=>
  string(22) "v_Teds\MutableIterable"
  [1]=>
  string(22) "v_Teds\MutableIterable"
}
Testing Teds\MutableIterable from Traversable
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
    string(22) "v_Teds\MutableIterable"
  }
}
count=2
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
    string(22) "v_Teds\MutableIterable"
  }
}
values: array(2) {
  [0]=>
  string(22) "v_Teds\MutableIterable"
  [1]=>
  string(22) "v_Teds\MutableIterable"
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
    string(20) "v_Teds\StrictHashMap"
  }
}
count=2
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
    string(20) "v_Teds\StrictHashMap"
  }
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictHashMap"
  [1]=>
  string(20) "v_Teds\StrictHashMap"
}
Testing Teds\StrictHashMap from Traversable
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
    string(20) "v_Teds\StrictHashMap"
  }
}
count=2
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
    string(20) "v_Teds\StrictHashMap"
  }
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictHashMap"
  [1]=>
  string(20) "v_Teds\StrictHashMap"
}

Testing Teds\StrictHashSet
object(Teds\StrictHashSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
count=1
object(Teds\StrictHashSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
Testing Teds\StrictHashSet from Traversable
object(Teds\StrictHashSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
count=1
object(Teds\StrictHashSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
}

Testing Teds\StrictMaxHeap
object(Teds\StrictMaxHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
count=2
object(Teds\StrictMaxHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
Testing Teds\StrictMaxHeap from Traversable
object(Teds\StrictMaxHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
count=2
object(Teds\StrictMaxHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictMaxHeap"
  [1]=>
  string(20) "v_Teds\StrictMaxHeap"
}

Testing Teds\StrictMinHeap
object(Teds\StrictMinHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "v_Teds\StrictMinHeap"
}
count=2
object(Teds\StrictMinHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "v_Teds\StrictMinHeap"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "v_Teds\StrictMinHeap"
}
Testing Teds\StrictMinHeap from Traversable
object(Teds\StrictMinHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "v_Teds\StrictMinHeap"
}
count=2
object(Teds\StrictMinHeap)#%d (2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "v_Teds\StrictMinHeap"
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictMinHeap"
  [1]=>
  string(20) "v_Teds\StrictMinHeap"
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
    string(28) "v_Teds\StrictSortedVectorMap"
  }
}
count=2
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
    string(28) "v_Teds\StrictSortedVectorMap"
  }
}
values: array(2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorMap"
  [1]=>
  string(28) "v_Teds\StrictSortedVectorMap"
}
Testing Teds\StrictSortedVectorMap from Traversable
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
    string(28) "v_Teds\StrictSortedVectorMap"
  }
}
count=2
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
    string(28) "v_Teds\StrictSortedVectorMap"
  }
}
values: array(2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorMap"
  [1]=>
  string(28) "v_Teds\StrictSortedVectorMap"
}

Testing Teds\StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#%d (1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
}
count=1
object(Teds\StrictSortedVectorSet)#%d (1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
}
values: array(1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
}
Testing Teds\StrictSortedVectorSet from Traversable
object(Teds\StrictSortedVectorSet)#%d (0) {
}
count=1
object(Teds\StrictSortedVectorSet)#%d (0) {
}
values: array(1) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
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
    string(20) "v_Teds\StrictTreeMap"
  }
}
count=2
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
    string(20) "v_Teds\StrictTreeMap"
  }
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictTreeMap"
  [1]=>
  string(20) "v_Teds\StrictTreeMap"
}
Testing Teds\StrictTreeMap from Traversable
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
    string(20) "v_Teds\StrictTreeMap"
  }
}
count=2
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
    string(20) "v_Teds\StrictTreeMap"
  }
}
values: array(2) {
  [0]=>
  string(20) "v_Teds\StrictTreeMap"
  [1]=>
  string(20) "v_Teds\StrictTreeMap"
}

Testing Teds\StrictTreeSet
object(Teds\StrictTreeSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
count=1
object(Teds\StrictTreeSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
Testing Teds\StrictTreeSet from Traversable
object(Teds\StrictTreeSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
count=1
object(Teds\StrictTreeSet)#%d (1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}
values: array(1) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
}

Testing Teds\Vector
object(Teds\Vector)#%d (2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "v_Teds\Vector"
}
count=2
object(Teds\Vector)#%d (2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "v_Teds\Vector"
}
values: array(2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "v_Teds\Vector"
}
Testing Teds\Vector from Traversable
object(Teds\Vector)#%d (2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "v_Teds\Vector"
}
count=2
object(Teds\Vector)#%d (2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "v_Teds\Vector"
}
values: array(2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  string(13) "v_Teds\Vector"
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
    string(21) "v_Teds\CachedIterable"
  }
}
count=2
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
    string(21) "v_Teds\CachedIterable"
  }
}
values: array(2) {
  [0]=>
  string(21) "v_Teds\CachedIterable"
  [1]=>
  string(21) "v_Teds\CachedIterable"
}
Testing Teds\CachedIterable from Traversable
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
    string(21) "v_Teds\CachedIterable"
  }
}
count=2
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
    string(21) "v_Teds\CachedIterable"
  }
}
values: array(2) {
  [0]=>
  string(21) "v_Teds\CachedIterable"
  [1]=>
  string(21) "v_Teds\CachedIterable"
}