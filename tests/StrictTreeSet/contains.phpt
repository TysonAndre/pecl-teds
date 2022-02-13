--TEST--
Teds\StrictTreeSet contains()
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$o = new stdClass();
$it = new Teds\StrictTreeSet(['first', $o, 'first']);
foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
    printf("%s: contains=%s\n", json_encode($value), json_encode($it->contains($value)));
}
?>
--EXPECT--
null: contains=false
"o": contains=false
{}: contains=true
{}: contains=false
"first": contains=true
