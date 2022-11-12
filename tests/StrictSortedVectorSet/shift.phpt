--TEST--
Teds\StrictSortedVectorSet shift/pop
--FILE--
<?php

$it = new Teds\StrictSortedVectorSet();
for ($i = 0; $i < 7; $i++) {
    $j = $i * 5 % 7;
    $it->add("v$j");
}
echo "shift\n";
echo $it->shift(), "\n";
echo $it->first(), "\n";
echo $it->shift(), "\n";
echo "pop\n";
echo $it->pop(), "\n";
echo $it->last(), "\n";
echo $it->pop(), "\n";
echo implode(',', $it->values()) . "\n";
echo $it->pop(), "\n";
echo $it->pop(), "\n";
echo $it->pop(), "\n";
echo "Empty\n";
foreach (['pop', 'shift', 'first', 'last'] as $method) {
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
pop: Caught: Cannot pop from empty Teds\StrictSortedVectorSet
shift: Caught: Cannot popFront from empty Teds\StrictSortedVectorSet
first: Caught: Cannot read first value of empty Teds\StrictSortedVectorSet
last: Caught: Cannot read last value of empty Teds\StrictSortedVectorSet
object(Teds\StrictSortedVectorSet)#1 (0) {
}
