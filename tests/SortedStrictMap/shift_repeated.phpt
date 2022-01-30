--TEST--
Teds\SortedStrictMap shift/pop
--FILE--
<?php

$it = new Teds\SortedStrictMap();
for ($i = 0; $i < 10; $i++) {
    $j = $i;
    $it["k$i"] = "v$i";
}
while (count($it) > 0) {
    echo json_encode($it->shift()), "\n";
}
echo "pop\n";
$it = new Teds\SortedStrictMap();
for ($i = 0; $i < 10; $i++) {
    $it["k$i"] = "v$i";
}
while (count($it) > 0) {
    echo json_encode($it->pop()), "\n";
}
?>
--EXPECT--
["k0","v0"]
["k1","v1"]
["k2","v2"]
["k3","v3"]
["k4","v4"]
["k5","v5"]
["k6","v6"]
["k7","v7"]
["k8","v8"]
["k9","v9"]
pop
["k9","v9"]
["k8","v8"]
["k7","v7"]
["k6","v6"]
["k5","v5"]
["k4","v4"]
["k3","v3"]
["k2","v2"]
["k1","v1"]
["k0","v0"]