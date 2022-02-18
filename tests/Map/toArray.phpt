--TEST--
Teds\Collection toArray()
--FILE--
<?php
function create_dynamic(string $key) { return "v_$key"; }

function test_to_array(string $class) {
    echo "Test $class\n";
    $it = new $class([create_dynamic('abc') => create_dynamic('x'), 'def' => new stdClass()]);
    var_dump($it->toArray());
    unset($it);
    $it = new $class();
    $it[[]] = 123;
    try {
        var_dump($it->toArray());
    } catch (TypeError $e) {
        echo "Caught: {$e->getMessage()}\n";
    }
}

foreach ([
    Teds\StrictSortedVectorMap::class,
    Teds\StrictTreeMap::class,
    Teds\StrictHashMap::class,
] as $class) {
    test_to_array($class);
}
?>
--EXPECT--
Test Teds\StrictSortedVectorMap
array(2) {
  ["def"]=>
  object(stdClass)#2 (0) {
  }
  ["v_abc"]=>
  string(3) "v_x"
}
Caught: Illegal offset type
Test Teds\StrictTreeMap
array(2) {
  ["def"]=>
  object(stdClass)#1 (0) {
  }
  ["v_abc"]=>
  string(3) "v_x"
}
Caught: Illegal offset type
Test Teds\StrictHashMap
array(2) {
  ["v_abc"]=>
  string(3) "v_x"
  ["def"]=>
  object(stdClass)#2 (0) {
  }
}
Caught: Illegal offset type