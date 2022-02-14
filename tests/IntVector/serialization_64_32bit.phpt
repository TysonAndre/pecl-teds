--TEST--
Teds\IntVector can be serialized and unserialized
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) echo "skip 32-bit only\n"; ?>
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
set_error_handler(function ($errno, $msg) { echo "Warning: $msg\n"; });

function test_serialize_values(...$values) {
    $vec = new Teds\IntVector();
    try {
        foreach ($values as $v) { $vec[] = $v; }
    } catch (TypeError $e) {
        echo "Caught {$e->getMessage()}\n";
        return;
    }
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
?>
--EXPECT--
[0,2147483647,-2147483648]
"O:14:\"Teds\\IntVector\":2:{i:0;i:3;i:1;s:12:\"\x00\x00\x00\x00\xff\xff\xff\x7f\x00\x00\x00\x80\";}"
Unserialize: [0,2147483647,-2147483648]

[2147483647]
"O:14:\"Teds\\IntVector\":2:{i:0;i:3;i:1;s:4:\"\xff\xff\xff\x7f\";}"
Unserialize: [2147483647]

Caught Illegal Teds\IntVector value type float
Caught Illegal Teds\IntVector value type float
Caught Illegal Teds\IntVector value type float