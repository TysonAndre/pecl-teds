--TEST--
Test stable_compare() function on typed property
--FILE--
<?php
class X {
    public function __construct(public int $x) {}
}
$x = new X(123);
$a = [];
$a[] = &$x->x;
var_dump(Teds\stable_compare($a, [123]));
var_dump(Teds\stable_compare($a, [122]));
var_dump(Teds\stable_compare($a, [125]));
--EXPECT--
int(0)
int(1)
int(-1)