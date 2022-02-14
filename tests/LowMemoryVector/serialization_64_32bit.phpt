--TEST--
Teds\LowMemoryVector can be serialized and unserialized
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) echo "skip 32-bit only\n"; ?>
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
test_serialize_values(1.5);
test_serialize_values(true, false, false, true, true);
test_serialize_values(true, false, null, null, true);
test_serialize_values(0x7fff_ffff);
test_serialize_values(0x8000_0000);  // due to the lack of strict_types this float is converted to an int and overflows in php. https://wiki.php.net/rfc/implicit-float-int-deprecate does not affect this.
test_serialize_values(-0x8000_0000, -1);  // this is serialized as a float
test_serialize_values(-0x8000_0001, 1);
test_serialize_values(0x0102030405060708, 0x7ffefdfcfbfaf0f9, 0);
?>
--EXPECT--
[0,2147483647,-2147483648]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:12:\"\x00\x00\x00\x00\xff\xff\xff\x7f\x00\x00\x00\x80\";}"
Unserialize: [0,2147483647,-2147483648]

[1.5]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:6;i:1;s:8:\"\x00\x00\x00\x00\x00\x00\xf8?\";}"
Unserialize: [1.5]

[true,false,false,true,true]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x19\x03\";}"
Unserialize: [true,false,false,true,true]

[true,false,null,null,true]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:1;i:1;s:3:\"[\x03\x03\";}"
Unserialize: [true,false,null,null,true]

[2147483647]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:4:\"\xff\xff\xff\x7f\";}"
Unserialize: [2147483647]

[2147483648]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:6;i:1;s:8:\"\x00\x00\x00\x00\x00\x00\xe0A\";}"
Unserialize: [2147483648]

[-2147483648,-1]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:7;i:1;a:2:{i:0;d:-2147483648;i:1;i:-1;}}"
Unserialize: [-2147483648,-1]

[-2147483649,1]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:7;i:1;a:2:{i:0;d:-2147483649;i:1;i:1;}}"
Unserialize: [-2147483649,1]

[72623859790382850,9.223088349902467e+18,0]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:7;i:1;a:3:{i:0;d:72623859790382850;i:1;d:9.223088349902467E+18;i:2;i:0;}}"
Unserialize: [72623859790382850,9.223088349902467e+18,0]