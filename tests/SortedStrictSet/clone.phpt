--TEST--
Teds\SortedStrictSet can be cloned
--FILE--
<?php

class A {}
class B {}
class C {}
$it = new Teds\SortedStrictSet([new A(), new B(), new C(), new A()]);
$it2 = clone $it;
unset($it);
foreach ($it2 as $value) {
    var_dump($value);
}
?>
--EXPECT--
object(A)#2 (0) {
}
object(A)#5 (0) {
}
object(B)#3 (0) {
}
object(C)#4 (0) {
}