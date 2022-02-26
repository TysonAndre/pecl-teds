--TEST--
Teds\BitVector can be serialized and unserialized
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
set_error_handler(function ($errno, $msg) { echo "Warning: $msg\n"; });

function test_serialize_values(...$values) {
    $vec = new Teds\BitVector();
    foreach ($values as $v) { $vec[] = $v; }
    echo json_encode($vec), "\n";
    $ser = serialize($vec);
    echo encode_raw_string($ser), "\n";
    echo "Unserialize: ", json_encode(unserialize($ser)), "\n\n";
    $vec->push(true, false);
    $vec->pop();
    $vec->pop();
    $ser2 = serialize($vec);
    if ($ser2 !== $ser) {
        printf("Serialization somehow changed: %s != %s\n", encode_raw_string($ser), encode_raw_string($ser2));
    }

}
test_serialize_values();
test_serialize_values(true);
test_serialize_values(false, true, true, true, false, true, false, true);
test_serialize_values(false, true, true, true, false, true, false, true, true);
?>
--EXPECT--
[]
"O:14:\"Teds\\BitVector\":0:{}"
Unserialize: []

[true]
"O:14:\"Teds\\BitVector\":1:{i:0;s:2:\"\x01\x07\";}"
Unserialize: [true]

[false,true,true,true,false,true,false,true]
"O:14:\"Teds\\BitVector\":1:{i:0;s:2:\"\xae\x00\";}"
Unserialize: [false,true,true,true,false,true,false,true]

[false,true,true,true,false,true,false,true,true]
"O:14:\"Teds\\BitVector\":1:{i:0;s:3:\"\xae\x01\x07\";}"
Unserialize: [false,true,true,true,false,true,false,true,true]