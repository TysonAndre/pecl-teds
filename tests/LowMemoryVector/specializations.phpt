--TEST--
Test LowMemoryVector specializations
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
function my_error_handler($errno, $errmsg) { echo "Warning: $errmsg\n"; }
set_error_handler('my_error_handler');

function test_low_memory_vector($value, ...$extra) {
    printf("test_low_memory_vector of %s\n", json_encode($value));
    $vec = new Teds\LowMemoryVector();
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
    [1.5, -2.5, 1, PHP_INT_MAX],
    [0x1234, -0x5421, true, PHP_INT_MAX],
    [0x12345678, -0x5421, PHP_INT_MAX],
    [true, false, null, create_str('x'), PHP_INT_MAX],
    [false, true],
    [null, false, true, create_str('x'), PHP_INT_MAX],
    [create_str('a'), 'b', create_str('a'), null],
] as $args) {
    test_low_memory_vector(...$args);
}
?>
--EXPECTF--
test_low_memory_vector of 1
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:2;i:1;s:2:\"\x01\x01\";}"
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

test_low_memory_vector of 1.5
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:6;i:1;s:16:\"\x00\x00\x00\x00\x00\x00\xf8?\x00\x00\x00\x00\x00\x00\xf8?\";}"
Unserialized: [1,1]
1.5: indexOf=0 contains=true
-2.5: indexOf=null contains=false
1: indexOf=null contains=false
%d: indexOf=null contains=false
Contents:float(1.5)
float(1.5)
Iterable values:[1.5,1.5]
Appending extra values
After appending extra values: [1.5,1.5,-2.5,1,%d]

test_low_memory_vector of 4660
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:3;i:1;s:4:\"4\x124\x12\";}"
Unserialized: [4660,4660]
4660: indexOf=0 contains=true
-21537: indexOf=null contains=false
true: indexOf=null contains=false
%d: indexOf=null contains=false
Contents:int(4660)
int(4660)
Iterable values:[4660,4660]
Appending extra values
After appending extra values: [4660,4660,-21537,true,%d]

test_low_memory_vector of 305419896
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:8:\"xV4\x12xV4\x12\";}"
Unserialized: [305419896,305419896]
305419896: indexOf=0 contains=true
-21537: indexOf=null contains=false
%d: indexOf=null contains=false
Contents:int(305419896)
int(305419896)
Iterable values:[305419896,305419896]
Appending extra values
After appending extra values: [305419896,305419896,-21537,%d]

test_low_memory_vector of true
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x02\x03\";}"
Unserialized: [false,true]
true: indexOf=0 contains=true
false: indexOf=null contains=false
null: indexOf=null contains=false
"_x": indexOf=null contains=false
%d: indexOf=null contains=false
Contents:bool(true)
bool(true)
Iterable values:[true,true]
Appending extra values
After appending extra values: [true,true,false,null,"_x",%d]

test_low_memory_vector of false
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:0;i:1;s:2:\"\x02\x00\";}"
Unserialized: [false,true]
false: indexOf=0 contains=true
true: indexOf=null contains=false
Contents:bool(false)
bool(false)
Iterable values:[false,false]
Appending extra values
After appending extra values: [false,false,true]

test_low_memory_vector of null
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:1;i:1;s:2:\"\x02\x05\";}"
Unserialized: [null,null]
null: indexOf=0 contains=true
false: indexOf=null contains=false
true: indexOf=null contains=false
"_x": indexOf=null contains=false
%d: indexOf=null contains=false
Contents:NULL
NULL
Iterable values:[null,null]
Appending extra values
After appending extra values: [null,null,false,true,"_x",%d]

test_low_memory_vector of "_a"
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:7;i:1;a:2:{i:0;s:2:\"_a\";i:1;s:2:\"_a\";}}"
Unserialized: ["_a","_a"]
"_a": indexOf=0 contains=true
"b": indexOf=null contains=false
"_a": indexOf=0 contains=true
null: indexOf=null contains=false
Contents:string(2) "_a"
string(2) "_a"
Iterable values:["_a","_a"]
Appending extra values
After appending extra values: ["_a","_a","b","_a",null]