--TEST--
Teds\ImmutableIterable containsValue()/indexOfValue();
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$o = new stdClass();
foreach ([Teds\ImmutableIterable::class, Teds\CachedIterable::class] as $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(['first', $o, 'first']);
    foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
        printf("%s: containsValue=%s, indexOfValue=%s\n", json_encode($value), json_encode($it->containsValue($value)), json_encode($it->indexOfValue($value)));
    }
    foreach ([0, 1, strtolower('FIRST')] as $key) {
        printf("%s: containsKey=%s, indexOfKey=%s\n", json_encode($key), json_encode($it->containsKey($key)), json_encode($it->indexOfKey($key)));
    }
}
?>
--EXPECT--
Test Teds\ImmutableIterable
null: containsValue=false, indexOfValue=null
"o": containsValue=false, indexOfValue=null
{}: containsValue=true, indexOfValue=1
{}: containsValue=false, indexOfValue=null
"first": containsValue=true, indexOfValue=0
0: containsKey=true, indexOfKey=0
1: containsKey=true, indexOfKey=1
"first": containsKey=false, indexOfKey=null
Test Teds\CachedIterable
null: containsValue=false, indexOfValue=null
"o": containsValue=false, indexOfValue=null
{}: containsValue=true, indexOfValue=1
{}: containsValue=false, indexOfValue=null
"first": containsValue=true, indexOfValue=0
0: containsKey=true, indexOfKey=0
1: containsKey=true, indexOfKey=1
"first": containsKey=false, indexOfKey=null