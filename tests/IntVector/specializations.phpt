--TEST--
Test IntVector specializations
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
function my_error_handler($errno, $errmsg) { echo "Warning: $errmsg\n"; }
set_error_handler('my_error_handler');

function test_low_memory_vector($value, ...$extra) {
    printf("test_low_memory_vector of %s\n", json_encode($value));
    $vec = new Teds\IntVector();
    $vec->push($value);
    $vec->push($value);
    $ser = serialize($vec);
    echo encode_raw_string($ser), "\n";
    printf("Unserialized: %s\n", json_encode(unserialize($ser)));
    foreach ([$value, ...$extra] as $v) {
        printf("%s: indexOf=%s contains=%s\n",
            json_encode($v),
            json_encode($vec->indexOf($v)),
            json_encode($vec->contains($v)),
        );
    }
    echo "Contents:";
    var_dump($vec[0]);
    var_dump($vec[1]);
    echo "Iterable values:" . json_encode(iterator_to_array($vec)) . "\n";
    echo "Appending extra values\n";
    $vec->push(...$extra);
    echo "After appending extra values: ", json_encode($vec), "\n";
    echo "\n";
}

function create_str(string $suffix) {
    return "_$suffix";
}
foreach ([
    [1, 2, 0x10001, PHP_INT_MAX],
    [0x10001, PHP_INT_MAX, PHP_INT_MIN],
    [0x12345678, -0x5421, PHP_INT_MAX],
] as $args) {
    test_low_memory_vector(...$args);
}
?>
--EXPECTF--
test_low_memory_vector of 1
"O:14:\"Teds\\IntVector\":2:{i:0;i:1;i:1;s:2:\"\x01\x01\";}"
Unserialized: [1,1]
1: indexOf=0 contains=true
2: indexOf=null contains=false
65537: indexOf=null contains=false
%d: indexOf=null contains=false
Contents:int(1)
int(1)
Iterable values:[1,1]
Appending extra values
After appending extra values: [1,1,2,65537,%d]

test_low_memory_vector of 65537
"O:14:\"Teds\\IntVector\":2:{i:0;i:3;i:1;s:8:\"\x01\x00\x01\x00\x01\x00\x01\x00\";}"
Unserialized: [65537,65537]
65537: indexOf=0 contains=true
%d: indexOf=null contains=false
-%d: indexOf=null contains=false
Contents:int(65537)
int(65537)
Iterable values:[65537,65537]
Appending extra values
After appending extra values: [65537,65537,%d,-%d]

test_low_memory_vector of 305419896
"O:14:\"Teds\\IntVector\":2:{i:0;i:3;i:1;s:8:\"xV4\x12xV4\x12\";}"
Unserialized: [305419896,305419896]
305419896: indexOf=0 contains=true
-21537: indexOf=null contains=false
%d: indexOf=null contains=false
Contents:int(305419896)
int(305419896)
Iterable values:[305419896,305419896]
Appending extra values
After appending extra values: [305419896,305419896,-21537,%d]