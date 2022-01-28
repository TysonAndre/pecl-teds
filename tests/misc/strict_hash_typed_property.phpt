--TEST--
Test strict_hash() function on typed property
--FILE--
<?php
class X {
    public function __construct(public int $x) {}
}
$x = new X(123);
$a = [];
$a[] = &$x->x;
var_dump(Teds\strict_hash($a) === Teds\strict_hash([123]));
--EXPECT--
bool(true)
