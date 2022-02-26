--TEST--
Teds\BitVector shift
--FILE--
<?php
function test_shift_bitvector(array $values, int $n) {
    $bitvector = new Teds\BitVector($values);
    var_dump(json_encode($values) === json_encode($bitvector));
    for ($i = 0; $i < $n; $i++) {
        $v1 = array_shift($values);
        $v2 = $bitvector->shift();
        if ($v1 !== $v2) {
            throw new RuntimeException(sprintf("%d: mismatch in BitVector shift: %s !== %s", $i, json_encode($v1), json_encode($v2)));
        }
    }
    echo "Shift $i was the same\n";
    var_dump(json_encode($values) === json_encode($bitvector));
}
test_shift_bitvector([true, false, true, true, true, true, true, false, true, false], 10);
$values = [];
for ($i = 0; $i < 64; $i++) {
    for ($j = 0, $v = $i; $j < 4; $j++, $v >>= 1) {
        $values[] = $v % 2 > 0;
    }
};
test_shift_bitvector($values, 70);
?>
--EXPECT--
bool(true)
Shift 10 was the same
bool(true)
bool(true)
Shift 70 was the same
bool(true)