--TEST--
Teds\Map converted to array.
--FILE--
<?php

function test_map_to_array(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name();
    var_dump((array) $it);
    $o1 = new stdClass();
    $o2 = new stdClass();
    $it[$o1] = $o2;
    var_dump((array) $it);
    echo "Replace with null/unset\n";
    $it[$o1] = null;
    var_dump((array) $it);
    unset($it[$o1]);
    var_dump((array) $it);
    var_dump($it);
    echo "\n";
}

foreach ([
    Teds\StrictHashMap::class,
    Teds\StrictTreeMap::class,
    Teds\StrictSortedVectorMap::class,
] as $class_name) {
    test_map_to_array($class_name);
}
?>
--EXPECT--
Test Teds\StrictHashMap
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
}
Replace with null/unset
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    NULL
  }
}
array(0) {
}
object(Teds\StrictHashMap)#1 (0) {
}

Test Teds\StrictTreeMap
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    object(stdClass)#1 (0) {
    }
  }
}
Replace with null/unset
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    NULL
  }
}
array(0) {
}
object(Teds\StrictTreeMap)#3 (0) {
}

Test Teds\StrictSortedVectorMap
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
}
Replace with null/unset
array(1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    NULL
  }
}
array(0) {
}
object(Teds\StrictSortedVectorMap)#1 (0) {
}