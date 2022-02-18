--TEST--
Teds\Map/Iterable::fromPairs
--FILE--
<?php

function test_collection_from_pairs(string $className) {
    try {
        $className::fromPairs(['first' => 'x']);
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    $it = $className::fromPairs([[0, 1], [2, 'a'], [1, 'b']]);
    foreach ($it as $key => $value) {
        printf("key=%s value=%s\n", json_encode($key), json_encode($value));
    }
    $it = $className::fromPairs([['first', 'x'], [(object)['key' => 'value'], null]]);
    foreach ($it as $key => $value) {
        printf("key=%s value=%s\n", json_encode($key), json_encode($value));
    }
    $pairsIterable = call_user_func(function () { yield new stdClass() => [strtolower('KEY'), strtolower('OTHER')]; });
    $it = $className::fromPairs($pairsIterable);
    foreach ($it as $key => $value) {
        printf("key=%s value=%s\n", json_encode($key), json_encode($value));
    }
    var_dump($it->toPairs());
}
foreach ([
    Teds\StrictSortedVectorMap::class,
    Teds\StrictTreeMap::class,
    Teds\StrictHashMap::class,
    Teds\ImmutableIterable::class,
    Teds\MutableIterable::class,
] as $class) {
    test_collection_from_pairs($class);
}

?>
--EXPECT--
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key=0 value=1
key=1 value="b"
key=2 value="a"
key="first" value="x"
key={"key":"value"} value=null
key="key" value="other"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "key"
    [1]=>
    string(5) "other"
  }
}
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key=0 value=1
key=1 value="b"
key=2 value="a"
key="first" value="x"
key={"key":"value"} value=null
key="key" value="other"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "key"
    [1]=>
    string(5) "other"
  }
}
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key=0 value=1
key=2 value="a"
key=1 value="b"
key="first" value="x"
key={"key":"value"} value=null
key="key" value="other"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "key"
    [1]=>
    string(5) "other"
  }
}
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key=0 value=1
key=2 value="a"
key=1 value="b"
key="first" value="x"
key={"key":"value"} value=null
key="key" value="other"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "key"
    [1]=>
    string(5) "other"
  }
}
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key=0 value=1
key=2 value="a"
key=1 value="b"
key="first" value="x"
key={"key":"value"} value=null
key="key" value="other"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "key"
    [1]=>
    string(5) "other"
  }
}