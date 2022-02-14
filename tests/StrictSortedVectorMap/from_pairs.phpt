--TEST--
Teds\StrictSortedVectorMap::fromPairs
--FILE--
<?php

try {
    Teds\StrictSortedVectorMap::fromPairs(['first' => 'x']);
} catch (Throwable $e) {
    printf("Caught %s: %s\n", $e::class, $e->getMessage());
}
$it = Teds\StrictSortedVectorMap::fromPairs([[0, 1], [2, 'a'], [1, 'b']]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
$it = Teds\StrictSortedVectorMap::fromPairs([['first', 'x'], [(object)['key' => 'value'], null]]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
$pairsIterable = call_user_func(function () { yield new stdClass() => [strtolower('KEY'), strtolower('OTHER')]; });
$it = Teds\StrictSortedVectorMap::fromPairs($pairsIterable);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
var_dump($it->toPairs());

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