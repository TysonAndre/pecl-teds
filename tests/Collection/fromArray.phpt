--TEST--
Teds\Collection constructed from array
--FILE--
<?php
// discards keys
function test_collection_from_array(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(['first' => "v_$class_name", "k_$class_name" => new stdClass()]);
    var_dump($it);
    var_dump((array)$it);
}

foreach ([
    Teds\CachedIterable::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\LowMemoryVector::class,
    Teds\MutableIterable::class,
    Teds\StrictHashMap::class,
    Teds\StrictHashSet::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictSortedVectorSet::class,
    Teds\StrictTreeMap::class,
    Teds\StrictTreeSet::class,
    Teds\Vector::class,
    Teds\Deque::class,
] as $class_name) {
    test_collection_from_array($class_name);
}

?>
--EXPECT--
Test Teds\CachedIterable
object(Teds\CachedIterable)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(21) "v_Teds\CachedIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    string(21) "k_Teds\CachedIterable"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(21) "v_Teds\CachedIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    string(21) "k_Teds\CachedIterable"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Test Teds\ImmutableIterable
object(Teds\ImmutableIterable)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(24) "v_Teds\ImmutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    string(24) "k_Teds\ImmutableIterable"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(24) "v_Teds\ImmutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    string(24) "k_Teds\ImmutableIterable"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Test Teds\ImmutableSequence
object(Teds\ImmutableSequence)#1 (2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(24) "v_Teds\ImmutableSequence"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
Test Teds\LowMemoryVector
object(Teds\LowMemoryVector)#1 (2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
Test Teds\MutableIterable
object(Teds\MutableIterable)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(22) "v_Teds\MutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    string(22) "k_Teds\MutableIterable"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(22) "v_Teds\MutableIterable"
  }
  [1]=>
  array(2) {
    [0]=>
    string(22) "k_Teds\MutableIterable"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Test Teds\StrictHashMap
object(Teds\StrictHashMap)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(20) "v_Teds\StrictHashMap"
  }
  [1]=>
  array(2) {
    [0]=>
    string(20) "k_Teds\StrictHashMap"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(20) "v_Teds\StrictHashMap"
  }
  [1]=>
  array(2) {
    [0]=>
    string(20) "k_Teds\StrictHashMap"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Test Teds\StrictHashSet
object(Teds\StrictHashSet)#1 (2) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(20) "v_Teds\StrictHashSet"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
Test Teds\StrictSortedVectorMap
object(Teds\StrictSortedVectorMap)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(28) "v_Teds\StrictSortedVectorMap"
  }
  [1]=>
  array(2) {
    [0]=>
    string(28) "k_Teds\StrictSortedVectorMap"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(28) "v_Teds\StrictSortedVectorMap"
  }
  [1]=>
  array(2) {
    [0]=>
    string(28) "k_Teds\StrictSortedVectorMap"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Test Teds\StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#1 (2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(28) "v_Teds\StrictSortedVectorSet"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
Test Teds\StrictTreeMap
object(Teds\StrictTreeMap)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(20) "v_Teds\StrictTreeMap"
  }
  [1]=>
  array(2) {
    [0]=>
    string(20) "k_Teds\StrictTreeMap"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(20) "v_Teds\StrictTreeMap"
  }
  [1]=>
  array(2) {
    [0]=>
    string(20) "k_Teds\StrictTreeMap"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Test Teds\StrictTreeSet
object(Teds\StrictTreeSet)#1 (2) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(20) "v_Teds\StrictTreeSet"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
Test Teds\Vector
object(Teds\Vector)#1 (2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(13) "v_Teds\Vector"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
Test Teds\Deque
object(Teds\Deque)#1 (2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(12) "v_Teds\Deque"
  [1]=>
  object(stdClass)#2 (0) {
  }
}