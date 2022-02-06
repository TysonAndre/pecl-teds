--TEST--
Test LowMemoryVector bitset serialization
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';

function test_low_memory_vector(...$values) {
    echo "Original: " . json_encode($values), "\n";
    $vec = new Teds\LowMemoryVector($values);
    $ser = serialize($vec);
    echo encode_raw_string($ser), "\n";
    echo json_encode(unserialize($ser)), "\n\n";
}

test_low_memory_vector(true,  true,  true,  true,  true,  true,  true,  true);
test_low_memory_vector(false, false, false, false, false, false, false, false);
test_low_memory_vector(true,  false, true,  true,  false, true,  false, true);
test_low_memory_vector(true,  false, true,  true,  false, true,  false);
test_low_memory_vector(true,  false, true,  true,  false, true,  false, true,  true);
test_low_memory_vector(true,  false, true,  true,  false, true,  false, true,  true,  null);
test_low_memory_vector(null, null, null, null, true, false, null, true);
--EXPECTF--
Original: [true,true,true,true,true,true,true,true]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x00\xff\";}"
[false,false,false,false,false,false,false,false]

Original: [false,false,false,false,false,false,false,false]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x00\x00\";}"
[false,false,false,false,false,false,false,false]

Original: [true,false,true,true,false,true,false,true]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x00\xad\";}"
[false,false,false,false,false,false,false,false]

Original: [true,false,true,true,false,true,false]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x07-\";}"
[true,true,true,false,false,false,false]

Original: [true,false,true,true,false,true,false,true,true]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:3:\"\x01\x01\xd6\";}"
[true,true,false,false,false,false,false,false,false]

Original: [true,false,true,true,false,true,false,true,true,null]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:1;i:1;s:4:\"\x02\x0b\xef~\";}"
[true,false,true,true,false,true,false,true,true,null]

Original: [null,null,null,null,true,false,null,true]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:1;i:1;s:3:\"\x00U\xdb\";}"
[null,null,null,null,true,false,null,true]