--TEST--
Teds\Map contains()
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
function test_map_contains(string $class_name) {
    echo "Test $class_name\n";
    $o = new stdClass();
    $it = new $class_name(['first', $o, 'first']);
    foreach ([null, $o, 'o', new stdClass(), strtolower('FIRST')] as $i => $value) {
        printf("%s: contains=%s\n", json_encode($value), json_encode($it->contains($value)));
        if ($i < 1) {
            printf("%s: containsValue=%s\n", json_encode($value), json_encode($it->containsValue($value)));
        }
    }
    foreach ([0, 1, strtolower('FIRST')] as $key) {
        printf("%s: containsKey=%s\n", json_encode($key), json_encode($it->containsKey($key)));
    }
}
foreach ([
    Teds\StrictTreeMap::class,
    Teds\StrictHashMap::class,
    Teds\StrictSortedVectorMap::class,
] as $class_name) {
    test_map_contains($class_name);
}
?>
--EXPECTF--
Test Teds\StrictTreeMap
null: contains=false

Deprecated: Method Teds\StrictTreeMap::containsValue() is deprecated in %s on line 10
null: containsValue=false
{}: contains=true
"o": contains=false
{}: contains=false
"first": contains=true
0: containsKey=true
1: containsKey=true
"first": containsKey=false
Test Teds\StrictHashMap
null: contains=false

Deprecated: Method Teds\StrictHashMap::containsValue() is deprecated in %s on line 10
null: containsValue=false
{}: contains=true
"o": contains=false
{}: contains=false
"first": contains=true
0: containsKey=true
1: containsKey=true
"first": containsKey=false
Test Teds\StrictSortedVectorMap
null: contains=false
null: containsValue=false
{}: contains=true
"o": contains=false
{}: contains=false
"first": contains=true
0: containsKey=true
1: containsKey=true
"first": containsKey=false