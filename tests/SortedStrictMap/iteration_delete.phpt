--TEST--
Teds\SortedStrictMap delete during iteration
--FILE--
<?php

$map = new Teds\SortedStrictMap(['first' => 'x', 'second' => new stdClass()]);
$it = $map->getIterator();
foreach ($it as $key => $value) {
    unset($map[$key]);
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
$it->rewind();
var_dump($map);
echo "After rewind\n";
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
foreach ($it as $key => $value) {
    unset($map[$key]);
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
var_dump($map);

?>
--EXPECT--
Key: 'first'
Value: 'x'
{"valid":false,"key":null,"current":null}
object(Teds\SortedStrictMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    string(6) "second"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
After rewind
{"valid":true,"key":"second","current":{}}
Key: 'second'
Value: (object) array(
)
{"valid":false,"key":null,"current":null}
object(Teds\SortedStrictMap)#1 (0) {
}