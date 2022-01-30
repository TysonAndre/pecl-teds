--TEST--
Teds\SortedStrictMap supports rebalancing
--FILE--
<?php

function create_key(int $i) {
    return "k$i";
}
$it = new Teds\SortedStrictMap();
for ($i = 10; $i < 30; $i++) {
    $it[create_key($i + 100)] = create_key($i);
    if (!$it->debugIsBalanced()) {
        printf("Unexpected: inserting %s caused tree to be unbalanced", var_export($i, true));
        echo json_encode($it->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
        return;
    }
}

while (count($it) > 0) {
    $value = $it->pop();
    printf("Removed: %s\n", json_encode($value));
    if (!$it->debugIsBalanced()) {
        printf("Unexpected: Removing %s caused tree to be unbalanced", json_encode($value));
        echo json_encode($it->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
        return;
    }
}

?>
--EXPECT--
Removed: ["k129","k29"]
Removed: ["k128","k28"]
Removed: ["k127","k27"]
Removed: ["k126","k26"]
Removed: ["k125","k25"]
Removed: ["k124","k24"]
Removed: ["k123","k23"]
Removed: ["k122","k22"]
Removed: ["k121","k21"]
Removed: ["k120","k20"]
Removed: ["k119","k19"]
Removed: ["k118","k18"]
Removed: ["k117","k17"]
Removed: ["k116","k16"]
Removed: ["k115","k15"]
Removed: ["k114","k14"]
Removed: ["k113","k13"]
Removed: ["k112","k12"]
Removed: ["k111","k11"]
Removed: ["k110","k10"]
