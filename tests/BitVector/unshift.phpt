--TEST--
Teds\BitVector shift
--FILE--
<?php
function test_unshift_bitvector(array $values, int $n) {
    $bitvector = new Teds\BitVector();
    foreach ($values as $i => $v) {
        $bitvector->unshift($v);
    }
    $other = array_reverse($values);
    var_dump(json_encode($other) === json_encode($bitvector));
}
test_unshift_bitvector([true, false, true, true, true, true, true, false, true, false], 10);
$values = [];
for ($i = 0; $i < 64; $i++) {
    for ($j = 0, $v = $i; $j < 4; $j++, $v >>= 1) {
        $values[] = $v % 2 > 0;
    }
};
test_unshift_bitvector($values, 70);
?>
--EXPECT--
bool(true)
bool(true)
