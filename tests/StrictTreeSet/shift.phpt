--TEST--
Teds\StrictTreeSet popFront/pop
--FILE--
<?php

$it = new Teds\StrictTreeSet();
for ($i = 0; $i < 7; $i++) {
    $j = $i * 5 % 7;
    $it->add("v$j");
}
echo "popFront\n";
echo $it->shift(), "\n";
echo $it->first(), "\n";
echo $it->popFront(), "\n";
echo "pop\n";
echo $it->pop(), "\n";
echo $it->last(), "\n";
echo $it->pop(), "\n";
echo implode(',', $it->values()) . "\n";
echo $it->pop(), "\n";
echo $it->pop(), "\n";
echo $it->pop(), "\n";
echo "Empty\n";
foreach (['pop', 'popFront', 'first', 'last'] as $method) {
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
popFront
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
popFront: Caught: Cannot popFront from empty StrictTreeSet
first: Caught: Cannot read first value of empty StrictTreeSet
last: Caught: Cannot read last value of empty StrictTreeSet
object(Teds\StrictTreeSet)#1 (0) {
}
