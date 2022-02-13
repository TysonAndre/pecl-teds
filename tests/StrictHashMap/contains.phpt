--TEST--
Teds\StrictHashMap contains()
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$o = new stdClass();
$it = new Teds\StrictHashMap(['first', $o, 'first']);
foreach ([null, $o, 'o', new stdClass(), strtolower('FIRST')] as $i => $value) {
    printf("%s: contains=%s\n", json_encode($value), json_encode($it->contains($value)));
    if ($i < 2) {
        printf("%s: containsValue=%s\n", json_encode($value), json_encode($it->containsValue($value)));
    }
}
foreach ([0, 1, strtolower('FIRST')] as $key) {
    printf("%s: containsKey=%s\n", json_encode($key), json_encode($it->containsKey($key)));
}
?>
--EXPECTF--
null: contains=false

Deprecated: Method Teds\StrictHashMap::containsValue() is deprecated in %s on line 8
null: containsValue=false
{}: contains=true

Deprecated: Method Teds\StrictHashMap::containsValue() is deprecated in %s on line 8
{}: containsValue=true
"o": contains=false
{}: contains=false
"first": contains=true
0: containsKey=true
1: containsKey=true
"first": containsKey=false