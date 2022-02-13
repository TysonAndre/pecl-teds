--TEST--
Teds\StrictTreeMap shift/pop repeated
--FILE--
<?php

$set = new Teds\StrictTreeSet();
for ($i = 10; $i < 30; $i++) {
    var_dump($set->add("k$i"));
}
while (count($set) > 0) {
    echo json_encode($set->shift()), "\n";
    if (!$set->debugIsBalanced()) {
        throw new RuntimeException("Imbalanced");
    }
}
echo "pop\n";
$set = new Teds\StrictTreeSet();
for ($i = 10; $i < 30; $i++) {
    var_dump($set->add("k$i"));
}
while (count($set) > 0) {
    echo json_encode($set->pop()), "\n";
    if (!$set->debugIsBalanced()) {
        throw new RuntimeException("Imbalanced");
    }
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
"k10"
"k11"
"k12"
"k13"
"k14"
"k15"
"k16"
"k17"
"k18"
"k19"
"k20"
"k21"
"k22"
"k23"
"k24"
"k25"
"k26"
"k27"
"k28"
"k29"
pop
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
"k29"
"k28"
"k27"
"k26"
"k25"
"k24"
"k23"
"k22"
"k21"
"k20"
"k19"
"k18"
"k17"
"k16"
"k15"
"k14"
"k13"
"k12"
"k11"
"k10"