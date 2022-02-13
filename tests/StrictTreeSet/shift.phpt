--TEST--
Teds\StrictTreeSet shift/pop
--FILE--
<?php

$it = new Teds\StrictTreeSet();
for ($i = 0; $i < 7; $i++) {
    $j = $i * 5 % 7;
    $it->add("v$j");
}
echo "shift\n";
echo $it->shift(), "\n";
echo $it->bottom(), "\n";
echo $it->shift(), "\n";
echo "pop\n";
echo $it->pop(), "\n";
echo $it->top(), "\n";
echo $it->pop(), "\n";
echo implode(',', $it->values()) . "\n";
echo $it->pop(), "\n";
echo $it->pop(), "\n";
echo $it->pop(), "\n";
echo "Empty\n";
foreach (['pop', 'shift', 'bottom', 'top'] as $method) {
    try {
        echo "$method: ";
        var_dump($it->{$method}());
    } catch (UnderflowException $e) {
        echo "Caught: {$e->getMessage()}\n";
    }
}
var_dump($it);

?>
--EXPECT--
shift
v0
v1
v1
pop
v6
v5
v5
v2,v3,v4
v4
v3
v2
Empty
pop: Caught: Cannot pop from empty StrictTreeSet
shift: Caught: Cannot shift from empty StrictTreeSet
bottom: Caught: Cannot read bottom of empty StrictTreeSet
top: Caught: Cannot read top of empty StrictTreeSet
object(Teds\StrictTreeSet)#1 (0) {
}