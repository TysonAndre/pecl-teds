--TEST--
Teds\LowMemoryVector can be serialized and unserialized
--SKIPIF--
<?php if (PHP_INT_SIZE < 8) echo "skip 64-bit only\n"; ?>
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
set_error_handler(function ($errno, $msg) { echo "Warning: $msg\n"; });

function test_serialize_values(...$values) {
    $vec = new Teds\LowMemoryVector();
    foreach ($values as $v) { $vec[] = $v; }
    echo json_encode($vec), "\n";
    $ser = serialize($vec);
    echo encode_raw_string($ser), "\n";
    echo "Unserialize: ", json_encode(unserialize($ser)), "\n\n";
}
test_serialize_values(0, PHP_INT_MAX, PHP_INT_MIN);
test_serialize_values(0x7fff_ffff);
test_serialize_values(0x8000_0000);
test_serialize_values(-0x8000_0000, -1);
test_serialize_values(-0x8000_0001, 1);
test_serialize_values(0x0102030405060708, 0x7ffefdfcfbfaf0f9, 0);
?>
--EXPECT--
[0,9223372036854775807,-9223372036854775808]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:5;i:1;s:24:\"\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\xff\xff\xff\x7f\x00\x00\x00\x00\x00\x00\x00\x80\";}"
Unserialize: [0,9223372036854775807,-9223372036854775808]

[2147483647]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:4:\"\xff\xff\xff\x7f\";}"
Unserialize: [2147483647]

[2147483648]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:5;i:1;s:8:\"\x00\x00\x00\x80\x00\x00\x00\x00\";}"
Unserialize: [2147483648]

[-2147483648,-1]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:8:\"\x00\x00\x00\x80\xff\xff\xff\xff\";}"
Unserialize: [-2147483648,-1]

[-2147483649,1]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:5;i:1;s:16:\"\xff\xff\xff\x7f\xff\xff\xff\xff\x01\x00\x00\x00\x00\x00\x00\x00\";}"
Unserialize: [-2147483649,1]

[72623859790382856,9223088349902467321,0]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:5;i:1;s:24:\"\x08\x07\x06\x05\x04\x03\x02\x01\xf9\xf0\xfa\xfb\xfc\xfd\xfe\x7f\x00\x00\x00\x00\x00\x00\x00\x00\";}"
Unserialize: [72623859790382856,9223088349902467321,0]