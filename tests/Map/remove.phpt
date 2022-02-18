--TEST--
Teds\Map add and remove
--FILE--
<?php

function test_map_add_remove(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name();
    $it[[]] = new stdClass();
    $it[[]] = new stdClass();
    $it[0] = "v_$class_name";
    foreach ($it as $key => $value) {
        printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
    }
    var_dump($it);
    echo "After removing key\n";
    unset($it[0]);
    printf("count=%d\n", count($it));
    var_dump($it);
    foreach ($it as $key => $value) {
        printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
    }
}
foreach ([
    Teds\StrictHashMap::class,
    Teds\StrictTreeMap::class,
    Teds\StableSortedListMap::class,
] as $class_name) {
    test_map_add_remove($class_name);
}

?>
--EXPECT--
Test Teds\StrictHashMap
Key: array (
)
Value: (object) array(
)
Key: 0
Value: 'v_Teds\\StrictHashMap'
object(Teds\StrictHashMap)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
  [1]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(20) "v_Teds\StrictHashMap"
  }
}
After removing key
count=1
object(Teds\StrictHashMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
}
Key: array (
)
Value: (object) array(
)
Test Teds\StrictTreeMap
Key: 0
Value: 'v_Teds\\StrictTreeMap'
Key: array (
)
Value: (object) array(
)
object(Teds\StrictTreeMap)#3 (2) {
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
    array(0) {
    }
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
After removing key
count=1
object(Teds\StrictTreeMap)#3 (1) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
Key: array (
)
Value: (object) array(
)
Test Teds\StableSortedListMap
Key: 0
Value: 'v_Teds\\StableSortedListMap'
Key: array (
)
Value: (object) array(
)
object(Teds\StrictSortedVectorMap)#2 (2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(26) "v_Teds\StableSortedListMap"
  }
  [1]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#1 (0) {
    }
  }
}
After removing key
count=1
object(Teds\StrictSortedVectorMap)#2 (1) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#1 (0) {
    }
  }
}
Key: array (
)
Value: (object) array(
)