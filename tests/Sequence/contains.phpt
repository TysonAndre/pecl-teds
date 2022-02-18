--TEST--
Teds\Sequence contains()/indexOf();
--FILE--
<?php
// Use strtolower to create values that must be garbage collected
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\ImmutableSequence::class,
    Teds\LowMemoryVector::class,
] as $class_name) {
    echo "Test $class_name\n";
    $o = new stdClass();
    $it = new $class_name(['first', $o, 'first']);
    foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
        printf("%s: contains=%s, indexOf=%s\n", json_encode($value), json_encode($it->contains($value)), json_encode($it->indexOf($value)));
    }
}
?>
--EXPECT--
Test Teds\Deque
null: contains=false, indexOf=null
"o": contains=false, indexOf=null
{}: contains=true, indexOf=1
{}: contains=false, indexOf=null
"first": contains=true, indexOf=0
Test Teds\Vector
null: contains=false, indexOf=null
"o": contains=false, indexOf=null
{}: contains=true, indexOf=1
{}: contains=false, indexOf=null
"first": contains=true, indexOf=0
Test Teds\ImmutableSequence
null: contains=false, indexOf=null
"o": contains=false, indexOf=null
{}: contains=true, indexOf=1
{}: contains=false, indexOf=null
"first": contains=true, indexOf=0
Test Teds\LowMemoryVector
null: contains=false, indexOf=null
"o": contains=false, indexOf=null
{}: contains=true, indexOf=1
{}: contains=false, indexOf=null
"first": contains=true, indexOf=0