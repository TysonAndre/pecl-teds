--TEST--
Teds\LowMemoryVector promote storage type
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) { echo "skip 32-bit only\n"; } ?>
--FILE--
<?php
// discards keys
function dump_index_of(Teds\LowMemoryVector $it, $value)  {
    printf("indexOf %s: %s\n", json_encode($value), json_encode($it->indexOf($value)));
}
$it = new Teds\LowMemoryVector();
$it->push(123);
echo json_encode($it), "\n";
echo json_encode(clone $it), "\n";
$it->push(PHP_INT_MAX);
echo json_encode($it), "\n";
echo json_encode(clone $it), "\n";
$it->push(strtoupper('test'));
echo json_encode($it), "\n";
echo json_encode(clone $it), "\n";
unset($it);
echo "Test promote int32 to mixed\n";
$it = new Teds\LowMemoryVector();
$it->push(-123);
echo json_encode($it), "\n";
dump_index_of($it, -123);
dump_index_of($it, -256 - 123);
$it->push(strtoupper('test'));
echo json_encode($it), "\n";
dump_index_of($it, -123);
dump_index_of($it, 'TEST');
unset($it);
?>
--EXPECT--
[123]
[123]
[123,2147483647]
[123,2147483647]
[123,2147483647,"TEST"]
[123,2147483647,"TEST"]
Test promote int32 to mixed
[-123]
indexOf -123: 0
indexOf -379: null
[-123,"TEST"]
indexOf -123: 0
indexOf "TEST": 1