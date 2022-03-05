--TEST--
Teds\StrictTreeSet delete during iteration
--FILE--
<?php

$set = new Teds\StrictTreeSet(['x', new stdClass()]);
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
echo "After add\n";
for ($i = 0; $i < 3; $i++) {
    $set->add("k$i");
}
echo json_encode(['valid' => $it->valid(), 'key' => $it->key(), 'current' => $it->current()]), "\n";
echo "After rewind again\n";
$it->rewind();
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
bool(true)
Value: (object) array(
)
{"valid":false,"key":null,"current":null}
object(Teds\StrictTreeSet)#1 (0) {
}
After rewind
{"valid":false,"key":null,"current":null}
After add
{"valid":false,"key":null,"current":null}
After rewind again
{"valid":true,"key":"k0","current":"k0"}
bool(true)
Key: 'k0'
Value: 'k0'
bool(true)
Key: 'k1'
Value: 'k1'
bool(true)
Key: 'k2'
Value: 'k2'
{"valid":false,"key":null,"current":null}
object(Teds\StrictTreeSet)#1 (0) {
}