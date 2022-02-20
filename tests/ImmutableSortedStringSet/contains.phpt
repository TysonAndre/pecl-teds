--TEST--
Teds\ImmutableSortedStringSet create
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
$create_key = fn (string $x) => "v_$x";

$string_set = new Teds\ImmutableSortedStringSet(new ArrayObject([]));
var_dump($string_set->contains('v_123'));
var_dump($string_set->indexOf('v_123'));
echo "Test non-empty\n";
$string_set = new Teds\ImmutableSortedStringSet(new ArrayObject([$create_key('123'), $create_key('456'), 'v_000']));
foreach (['v_123', $create_key(456), $create_key('457'), 123] as $key) {
    printf("key=%s: contains=%s indexOf=%s\n", $key, json_encode($string_set->contains($key)), json_encode($string_set->indexOf($key)));
}
--EXPECT--
bool(false)
NULL
Test non-empty
key=v_123: contains=true indexOf=1
key=v_456: contains=true indexOf=2
key=v_457: contains=false indexOf=null
key=123: contains=false indexOf=null