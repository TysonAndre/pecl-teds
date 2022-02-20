--TEST--
Teds\ImmutableSortedStringSet create
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
function test_create(iterable $values) {
    $it = new Teds\ImmutableSortedStringSet($values);
    echo json_encode($it), "\n";
    printf("count=%d\n", count($it));
    $ser = serialize($it);
    echo "Serialized: ", encode_raw_string($ser), "\n";;
    $result = unserialize($ser);
    var_export($result);
    echo "\n";
    unset($result);
    $data_bytes = $it->serialize();
    echo "serialize(): ", encode_raw_string($data_bytes), "\n";;
    $result2 = Teds\ImmutableSortedStringSet::unserialize($data_bytes);
    $data_bytes[7] = "?";  // This does nothing, PHP strings are copy on write
    var_export($result2);
    echo "\n";
    var_dump($data_bytes !== $it->serialize());
}
$create_key = fn (string $x) => "v_$x";

test_create([]);
test_create(['']);
test_create([$create_key('a')]);
test_create([$create_key('a'), $create_key('a')]);
test_create(['b', 'a', $create_key('a'), '', 'a']);
test_create([str_repeat('abc', 100)]);
--EXPECT--
[]
count=0
Serialized: "O:29:\"Teds\\ImmutableSortedStringSet\":0:{}"
Teds\ImmutableSortedStringSet::__set_state(array(
))
serialize(): ""
Teds\ImmutableSortedStringSet::__set_state(array(
))
bool(true)
[""]
count=1
Serialized: "O:29:\"Teds\\ImmutableSortedStringSet\":1:{i:0;s:5:\"\x01\x00\x00\x00\x00\";}"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => '',
))
serialize(): "\x01\x00\x00\x00\x00"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => '',
))
bool(true)
["v_a"]
count=1
Serialized: "O:29:\"Teds\\ImmutableSortedStringSet\":1:{i:0;s:8:\"\x01\x00\x00\x00\x03v_a\";}"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => 'v_a',
))
serialize(): "\x01\x00\x00\x00\x03v_a"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => 'v_a',
))
bool(true)
["v_a"]
count=1
Serialized: "O:29:\"Teds\\ImmutableSortedStringSet\":1:{i:0;s:8:\"\x01\x00\x00\x00\x03v_a\";}"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => 'v_a',
))
serialize(): "\x01\x00\x00\x00\x03v_a"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => 'v_a',
))
bool(true)
["","a","b","v_a"]
count=4
Serialized: "O:29:\"Teds\\ImmutableSortedStringSet\":1:{i:0;s:13:\"\x04\x00\x00\x00\x00\x01a\x01b\x03v_a\";}"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => '',
   1 => 'a',
   2 => 'b',
   3 => 'v_a',
))
serialize(): "\x04\x00\x00\x00\x00\x01a\x01b\x03v_a"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => '',
   1 => 'a',
   2 => 'b',
   3 => 'v_a',
))
bool(true)
["abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc"]
count=1
Serialized: "O:29:\"Teds\\ImmutableSortedStringSet\":1:{i:0;s:307:\"\x01\x00\x00\x00\xfd,\x01abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc\";}"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => 'abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc',
))
serialize(): "\x01\x00\x00\x00\xfd,\x01abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc"
Teds\ImmutableSortedStringSet::__set_state(array(
   0 => 'abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc',
))
bool(true)