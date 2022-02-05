--TEST--
Teds\LowMemoryVector contains()/indexOf();
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
$o = new stdClass();
$it = new Teds\LowMemoryVector(['first', $o, 'first']);
foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
    printf("%s: contains=%s, indexOf=%s\n", json_encode($value), json_encode($it->contains($value)), json_encode($it->indexOf($value)));
}
?>
--EXPECT--
null: contains=false, indexOf=null
"o": contains=false, indexOf=null
{}: contains=true, indexOf=1
{}: contains=false, indexOf=null
"first": contains=true, indexOf=0