--TEST--
Teds\StrictHashMap offset access
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
    try {
        $it[] = 123;
        echo "Should fail\n";
    } catch (RuntimeException $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    unset($v, $v2, $o, $o2, $o3);
    // Should convert top-level references to values.
    $v = 123;
    $v2 = &$v;
    $o = new stdClass();
    $o2 = &$o;
    $it[$o2] = $v;
    echo "isset: ";
    var_dump(isset($it[$o2]));
    echo "empty: ";
    var_dump(empty($it[$o2]));

    var_dump($it);
    $o3 = $o2;
    $o2 = 'replace key';
    $v = 'replace value';
    echo "After replacing original references this is unchanged\n";
    var_dump($it);

    unset($it[$o3]);
    echo "After unset:\n";
    var_dump($it);
    $it[$undefVar] = 123;
    var_dump(isset($it[$undefVar2]));
}

?>
--EXPECTF--
Test Teds\StrictHashMap
Caught RuntimeException: Teds\StrictHashMap does not support appending with []=
isset: bool(true)
empty: bool(false)
object(Teds\StrictHashMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#3 (0) {
    }
    [1]=>
    int(123)
  }
}
After replacing original references this is unchanged
object(Teds\StrictHashMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#3 (0) {
    }
    [1]=>
    int(123)
  }
}
After unset:
object(Teds\StrictHashMap)#1 (0) {
}

Warning: Undefined variable $undefVar in %s on line 39

Warning: Undefined variable $undefVar2 in %s on line 40
bool(true)
Test Teds\StrictTreeMap
Caught RuntimeException: Teds\StrictHashMap does not support appending with []=
isset: bool(true)
empty: bool(false)
object(Teds\StrictTreeMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#3 (0) {
    }
    [1]=>
    int(123)
  }
}
After replacing original references this is unchanged
object(Teds\StrictTreeMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#3 (0) {
    }
    [1]=>
    int(123)
  }
}
After unset:
object(Teds\StrictTreeMap)#1 (0) {
}

Warning: Undefined variable $undefVar in %s on line 39

Warning: Undefined variable $undefVar2 in %s on line 40
bool(true)
Test Teds\StrictSortedVectorMap
Caught RuntimeException: Teds\StrictSortedVectorMap does not support appending with []=
isset: bool(true)
empty: bool(false)
object(Teds\StrictSortedVectorMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#3 (0) {
    }
    [1]=>
    int(123)
  }
}
After replacing original references this is unchanged
object(Teds\StrictSortedVectorMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#3 (0) {
    }
    [1]=>
    int(123)
  }
}
After unset:
object(Teds\StrictSortedVectorMap)#1 (0) {
}

Warning: Undefined variable $undefVar in %s on line 39

Warning: Undefined variable $undefVar2 in %s on line 40
bool(true)