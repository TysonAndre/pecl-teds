--TEST--
Teds\StrictTreeMap shift/pop
--FILE--
<?php

$it = new Teds\StrictTreeMap();
for ($i = 0; $i < 7; $i++) {
    $j = $i * 5 % 7;
    $it["k$j"] = "v$j";
    if (!$it->debugIsBalanced()) {
        echo json_encode($it->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
        return;
    }
}
echo "shift\n";
echo json_encode($it->shift()), "\n";
echo json_encode($it->bottom()), "\n";
echo json_encode($it->shift()), "\n";
echo "pop\n";
echo json_encode($it->pop()), "\n";
echo json_encode($it->top()), "\n";
echo json_encode($it->pop()), "\n";
echo implode(',', $it->values()) . "\n";
echo json_encode($it->pop()), "\n";
echo json_encode($it->pop()), "\n";
echo json_encode($it->pop()), "\n";
echo "Empty\n";
foreach (['shift', 'pop', 'bottom', 'top'] as $method) {
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
shift: Caught: Cannot shift from empty StrictTreeMap
pop: Caught: Cannot pop from empty StrictTreeMap
bottom: Caught: Cannot read bottom of empty StrictTreeMap
top: Caught: Cannot read top of empty StrictTreeMap
object(Teds\StrictTreeMap)#1 (0) {
}