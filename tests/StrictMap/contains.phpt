--TEST--
Teds\StrictMap containsValue()
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$o = new stdClass();
$it = new Teds\StrictMap(['first', $o, 'first']);
foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
    printf("%s: containsValue=%s\n", json_encode($value), json_encode($it->containsValue($value)));
}
foreach ([0, 1, strtolower('FIRST')] as $key) {
    printf("%s: containsKey=%s\n", json_encode($key), json_encode($it->containsKey($key)));
}
?>
--EXPECT--
null: containsValue=false
"o": containsValue=false
{}: containsValue=true
{}: containsValue=false
"first": containsValue=true
0: containsKey=true
1: containsKey=true
"first": containsKey=false