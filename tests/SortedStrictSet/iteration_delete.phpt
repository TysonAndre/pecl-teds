--TEST--
Teds\SortedStrictSet delete during iteration
--FILE--
<?php

$set = new Teds\SortedStrictSet(['x', new stdClass()]);
$it = $set->getIterator();
foreach ($it as $value) {
    var_dump($set->remove($value));
    printf("Value: %s\n", var_export($value, true));
}
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
$it->rewind();
var_dump($set);
echo "After rewind\n";
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
foreach ($it as $key => $value) {
    var_dump($set->remove($value));
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
var_dump($set);

?>
--EXPECT--
bool(true)
Value: 'x'
{"valid":false,"key":null,"current":null}
object(Teds\SortedStrictSet)#1 (1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
After rewind
{"valid":true,"key":{},"current":{}}
bool(true)
Key: (object) array(
)
Value: (object) array(
)
{"valid":false,"key":null,"current":null}
object(Teds\SortedStrictSet)#1 (0) {
}