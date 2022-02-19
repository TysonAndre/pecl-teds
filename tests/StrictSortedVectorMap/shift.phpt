--TEST--
Teds\StrictSortedVectorMap shift/pop
--FILE--
<?php

$it = new Teds\StrictSortedVectorMap();
for ($i = 0; $i < 7; $i++) {
    $j = $i * 5 % 7;
    $it["k$j"] = "v$j";
}
echo "shift\n";
echo json_encode($it->shift()), "\n";
echo json_encode($it->first()), "\n";
echo json_encode($it->shift()), "\n";
echo "pop\n";
echo json_encode($it->pop()), "\n";
echo json_encode($it->last()), "\n";
echo json_encode($it->pop()), "\n";
echo implode(',', $it->values()) . "\n";
echo json_encode($it->pop()), "\n";
echo json_encode($it->pop()), "\n";
echo json_encode($it->pop()), "\n";
echo "Empty\n";
foreach (['shift', 'pop', 'first', 'last'] as $method) {
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
["k0","v0"]
"v1"
["k1","v1"]
pop
["k6","v6"]
"v5"
["k5","v5"]
v2,v3,v4
["k4","v4"]
["k3","v3"]
["k2","v2"]
Empty
shift: Caught: Cannot shift from empty StrictSortedVectorMap
pop: Caught: Cannot pop from empty StrictSortedVectorMap
first: Caught: Cannot read first of empty StrictSortedVectorMap
last: Caught: Cannot read last of empty StrictSortedVectorMap
object(Teds\StrictSortedVectorMap)#1 (0) {
}