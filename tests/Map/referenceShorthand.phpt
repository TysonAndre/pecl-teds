--TEST--
Teds\StrictHashMap ArrayAccess forbids reference
--FILE--
<?php

foreach ([
    Teds\StrictHashMap::class,
    Teds\StrictTreeMap::class,
    Teds\StrictSortedVectorMap::class,
] as $class_name) {
    unset($it);
    echo "Test $class_name\n";
    $it = new $class_name();
    $value = new ArrayObject();
    try {
        $it[123] = &$value;
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    $value2 = &$value;
    $it["v2_$class_name"] ??= $value;
    $value2 = 'replaced'; // no effect.
    $it["v2_$class_name"][1] = "dynamic_$class_name";
    var_dump($it);
}

?>
--EXPECT--
Test Teds\StrictHashMap
Caught OutOfBoundsException: Key not found
object(Teds\StrictHashMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    string(21) "v2_Teds\StrictHashMap"
    [1]=>
    object(ArrayObject)#2 (1) {
      ["storage":"ArrayObject":private]=>
      array(1) {
        [1]=>
        string(26) "dynamic_Teds\StrictHashMap"
      }
    }
  }
}
Test Teds\StrictTreeMap
Caught OutOfBoundsException: Key not found
object(Teds\StrictTreeMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    string(21) "v2_Teds\StrictTreeMap"
    [1]=>
    object(ArrayObject)#2 (1) {
      ["storage":"ArrayObject":private]=>
      array(1) {
        [1]=>
        string(26) "dynamic_Teds\StrictTreeMap"
      }
    }
  }
}
Test Teds\StrictSortedVectorMap
Caught OutOfBoundsException: Key not found
object(Teds\StrictSortedVectorMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    string(29) "v2_Teds\StrictSortedVectorMap"
    [1]=>
    object(ArrayObject)#2 (1) {
      ["storage":"ArrayObject":private]=>
      array(1) {
        [1]=>
        string(34) "dynamic_Teds\StrictSortedVectorMap"
      }
    }
  }
}