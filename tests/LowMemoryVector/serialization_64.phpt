--TEST--
Teds\LowMemoryVector can be serialized and unserialized
--SKIPIF--
<?php if (PHP_INT_SIZE < 8) echo "skip 64-bit only\n"; ?>
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';

function test_serialize_values(...$values) {
    $vec = new Teds\LowMemoryVector();
    foreach ($values as $v) { $vec[] = $v; }
    echo json_encode($vec), "\n";
    echo encode_raw_string(serialize($vec)), "\n";
}
test_serialize_values(0, PHP_INT_MAX, PHP_INT_MIN);
test_serialize_values(0x80000000);
test_serialize_values(0x0102030405060708, 0x7ffefdfcfbfaf0f9, 0);
?>
--EXPECTF--
[0,9223372036854775807,-9223372036854775808]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:24:\"\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\x7f\x00\x00\x00\x00\x00\x00\x00\x80\";}"
[2147483648]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:8:\"\x00\x00\x00\x80\x00\x00\x00\x00\";}"
[72623859790382856,9223088349902467321,0]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:24:\"\x08\x07\x06\x05\x04\x03\x02\x01\xf9\xf0\xfa\xfb\xfc\xfd\xfe\x7f\x00\x00\x00\x00\x00\x00\x00\x00\";}"