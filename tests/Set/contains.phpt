--TEST--
Teds\Set contains()
--FILE--
<?php
foreach ([
    Teds\StrictTreeSet::class,
    Teds\StrictHashSet::class,
    Teds\StrictSortedVectorSet::class,
] as $class_name) {
    echo "Test $class_name\n";
    $o = new stdClass();
    $it = new $class_name(['first', $o, 'first']);
    foreach ([null, 'o', $o, new stdClass(), strtolower('FIRST')] as $value) {
        printf("%s: contains=%s\n", json_encode($value), json_encode($it->contains($value)));
    }
    unset($it);
};
?>
--EXPECT--
Test Teds\StrictTreeSet
null: contains=false
"o": contains=false
{}: contains=true
{}: contains=false
"first": contains=true
Test Teds\StrictHashSet
null: contains=false
"o": contains=false
{}: contains=true
{}: contains=false
"first": contains=true
Test Teds\StrictSortedVectorSet
null: contains=false
"o": contains=false
{}: contains=true
{}: contains=false
"first": contains=true
