--TEST--
Teds\IntVector promote storage type
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) { echo "skip 32-bit only\n"; } ?>
--FILE--
<?php
// discards keys
function dump_index_of(Teds\IntVector $it, int $value)  {
    printf("indexOf %s: %s\n", json_encode($value), json_encode($it->indexOf($value)));
}
$it = new Teds\IntVector();
$it->push(123);
echo json_encode($it), "\n";
echo json_encode(clone $it), "\n";
$it->push(PHP_INT_MAX);
echo json_encode($it), "\n";
echo json_encode(clone $it), "\n";
unset($it);
echo "Test promote int32 to mixed\n";
$it = new Teds\IntVector();
$it->push(-123);
echo json_encode($it), "\n";
dump_index_of($it, -123);
dump_index_of($it, -256 - 123);
unset($it);
?>
--EXPECT--
[123]
[123]
[123,2147483647]
[123,2147483647]
Test promote int32 to mixed
[-123]
indexOf -123: 0
indexOf -379: null