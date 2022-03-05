--TEST--
Teds\StrictTreeMap delete during iteration
--FILE--
<?php

$map = new Teds\StrictTreeMap(['first' => 'x', 'second' => new stdClass()]);
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
echo "After add\n";
for ($i = 0; $i < 3; $i++) {
    $map["k$i"] = "v$i";
}
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
echo "After another rewind\n";
$it->rewind();
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
Key: 'second'
Value: (object) array(
)
{"valid":false,"key":null,"current":null}
object(Teds\StrictTreeMap)#1 (0) {
}
After rewind
{"valid":false,"key":null,"current":null}
After add
{"valid":false,"key":null,"current":null}
After another rewind
{"valid":true,"key":"k0","current":"v0"}
Key: 'k0'
Value: 'v0'
Key: 'k1'
Value: 'v1'
Key: 'k2'
Value: 'v2'
{"valid":false,"key":null,"current":null}
object(Teds\StrictTreeMap)#1 (0) {
}