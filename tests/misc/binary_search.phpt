--TEST--
binary_search test
--FILE--
<?php

$values = [0, 1, 2, 5];
echo json_encode($values), "\n";
$dump = function (...$args) use ($values) {
};
foreach ([-1, 0, 2, 3, 5, 10] as $target) {
    echo json_encode($target) . ': ' . json_encode(Teds\binary_search($values, $target)) . "\n";
}
foreach ([-1, 0, 3, 9] as $targetKey) {
    echo json_encode($targetKey) . ' key: ' . json_encode(Teds\binary_search($values, $targetKey, null, true)) . "\n";
}
echo 'Empty: ' . json_encode(Teds\binary_search([], new stdClass())) . "\n";

function create_key($i) {
    return "k$i";
}
function create_value($i) {
    return "v$i";
}
$values = [];
for ($i = 0; $i < 10; $i += 2) {
    $values[create_key($i)] = create_value($i);
}
foreach (['+', 0, 3, 5, 9] as $i) {
    $targetKey = create_key($i);
    echo json_encode($targetKey) . ' key my_strcmp: ' . json_encode(Teds\binary_search($values, $targetKey, 'my_strcmp', true)) . "\n";
}
echo "remove k4 and k6\n";
unset($values['k4']);
unset($values['k6']);

echo json_encode($values), "\n";
foreach (['+', 0, 3, 5, 9] as $i) {
    $targetValue = create_value($i);
    echo json_encode($targetValue) . ' value: ' . json_encode(Teds\binary_search($values, $targetValue, null, false)) . "\n";
}
foreach (['+', 0, 3, 5, 9] as $i) {
    $targetKey = create_key($i);
    echo json_encode($targetKey) . ' key: ' . json_encode(Teds\binary_search($values, $targetKey, null, true)) . "\n";
}
function my_strcmp($target, $other): int {
    echo "Called my_strcmp($target, $other)\n";
    return strcmp($target, $other);
}
foreach (['+', 0, 3, 5, 9] as $i) {
    $targetKey = create_key($i);
    echo json_encode($targetKey) . ' key my_strcmp: ' . json_encode(Teds\binary_search($values, $targetKey, 'my_strcmp', true)) . "\n";
}
$values = [100,101,102,103,104,105,106,107,108,109,110,111,112];
foreach ([-1, 0, 3, 6, 12, 13] as $i) {
    echo json_encode($i + 100) . ' packed value: ' . json_encode(Teds\binary_search($values, $i + 100)) . "\n";
}
foreach ([-1, 0, 3, 6, 12, 13] as $i) {
    echo json_encode($i) . ' packed key: ' . json_encode(Teds\binary_search($values, $i, null, true)) . "\n";
}
unset($values[1]);
unset($values[2]);
unset($values[6]);
unset($values[7]);
unset($values[8]);
unset($values[9]);
unset($values[10]);
unset($values[11]);
echo json_encode($values), "\n";
foreach ([-1, 0, 3, 6, 12, 13] as $i) {
    echo json_encode($i + 100) . ' packed value gaps: ' . json_encode(Teds\binary_search($values, $i + 100)) . "\n";
}
foreach ([-1, 0, 3, 6, 12, 13] as $i) {
    echo json_encode($i) . ' packed key gaps: ' . json_encode(Teds\binary_search($values, $i, null, true)) . "\n";
}
?>
--EXPECT--
[0,1,2,5]
-1: {"found":false,"key":null,"value":null}
0: {"found":true,"key":0,"value":0}
2: {"found":true,"key":2,"value":2}
3: {"found":false,"key":2,"value":2}
5: {"found":true,"key":3,"value":5}
10: {"found":false,"key":3,"value":5}
-1 key: {"found":false,"key":null,"value":null}
0 key: {"found":true,"key":0,"value":0}
3 key: {"found":true,"key":3,"value":5}
9 key: {"found":false,"key":3,"value":5}
Empty: {"found":false,"key":null,"value":null}
Called my_strcmp(k+, k4)
Called my_strcmp(k+, k2)
Called my_strcmp(k+, k0)
"k+" key my_strcmp: {"found":false,"key":null,"value":null}
Called my_strcmp(k0, k4)
Called my_strcmp(k0, k2)
Called my_strcmp(k0, k0)
"k0" key my_strcmp: {"found":true,"key":"k0","value":"v0"}
Called my_strcmp(k3, k4)
Called my_strcmp(k3, k2)
"k3" key my_strcmp: {"found":false,"key":"k2","value":"v2"}
Called my_strcmp(k5, k4)
Called my_strcmp(k5, k8)
Called my_strcmp(k5, k6)
"k5" key my_strcmp: {"found":false,"key":"k4","value":"v4"}
Called my_strcmp(k9, k4)
Called my_strcmp(k9, k8)
"k9" key my_strcmp: {"found":false,"key":"k8","value":"v8"}
remove k4 and k6
{"k0":"v0","k2":"v2","k8":"v8"}
"v+" value: {"found":false,"key":null,"value":null}
"v0" value: {"found":true,"key":"k0","value":"v0"}
"v3" value: {"found":false,"key":"k2","value":"v2"}
"v5" value: {"found":false,"key":"k2","value":"v2"}
"v9" value: {"found":false,"key":"k8","value":"v8"}
"k+" key: {"found":false,"key":null,"value":null}
"k0" key: {"found":true,"key":"k0","value":"v0"}
"k3" key: {"found":false,"key":"k2","value":"v2"}
"k5" key: {"found":false,"key":"k2","value":"v2"}
"k9" key: {"found":false,"key":"k8","value":"v8"}
Called my_strcmp(k+, k8)
Called my_strcmp(k+, k2)
Called my_strcmp(k+, k0)
"k+" key my_strcmp: {"found":false,"key":null,"value":null}
Called my_strcmp(k0, k8)
Called my_strcmp(k0, k2)
Called my_strcmp(k0, k0)
"k0" key my_strcmp: {"found":true,"key":"k0","value":"v0"}
Called my_strcmp(k3, k8)
Called my_strcmp(k3, k2)
"k3" key my_strcmp: {"found":false,"key":"k2","value":"v2"}
Called my_strcmp(k5, k8)
Called my_strcmp(k5, k2)
"k5" key my_strcmp: {"found":false,"key":"k2","value":"v2"}
Called my_strcmp(k9, k8)
"k9" key my_strcmp: {"found":false,"key":"k8","value":"v8"}
99 packed value: {"found":false,"key":null,"value":null}
100 packed value: {"found":true,"key":0,"value":100}
103 packed value: {"found":true,"key":3,"value":103}
106 packed value: {"found":true,"key":6,"value":106}
112 packed value: {"found":true,"key":12,"value":112}
113 packed value: {"found":false,"key":12,"value":112}
-1 packed key: {"found":false,"key":null,"value":null}
0 packed key: {"found":true,"key":0,"value":100}
3 packed key: {"found":true,"key":3,"value":103}
6 packed key: {"found":true,"key":6,"value":106}
12 packed key: {"found":true,"key":12,"value":112}
13 packed key: {"found":false,"key":12,"value":112}
{"0":100,"3":103,"4":104,"5":105,"12":112}
99 packed value gaps: {"found":false,"key":null,"value":null}
100 packed value gaps: {"found":true,"key":0,"value":100}
103 packed value gaps: {"found":true,"key":3,"value":103}
106 packed value gaps: {"found":false,"key":5,"value":105}
112 packed value gaps: {"found":true,"key":12,"value":112}
113 packed value gaps: {"found":false,"key":12,"value":112}
-1 packed key gaps: {"found":false,"key":null,"value":null}
0 packed key gaps: {"found":true,"key":0,"value":100}
3 packed key gaps: {"found":true,"key":3,"value":103}
6 packed key gaps: {"found":false,"key":5,"value":105}
12 packed key gaps: {"found":true,"key":12,"value":112}
13 packed key gaps: {"found":false,"key":12,"value":112}