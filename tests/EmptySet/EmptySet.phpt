--TEST--
Test Teds\EmptySet
--SKIPIF--
<?php if (PHP_VERSION_ID < 80100) echo "skip php 8.1+ enums required\n"; ?>
--FILE--
<?php
use Teds\EmptySet;
$s = Teds\EmptySet::INSTANCE;
function expect_throws(Closure $c) {
    try {
        $c();
        echo "Unexpectedly didn't throw\n";
    } catch (Throwable $t) {
        printf("Caught %s: %s\n", $t::class, $t->getMessage());
    }
}
var_dump($s);
var_dump($s->isEmpty());
var_dump($s->clear());
var_dump($s->toArray());
echo "Test iterable methods\n";
var_dump($s->next());
var_dump($s->rewind());
var_dump($s->valid());
var_dump($s->remove(123));
expect_throws(fn() => $s->current());
expect_throws(fn() => $s->add(new stdClass()));
expect_throws(fn() => $s->key());
expect_throws(fn() => clone($s));
var_export($s);
echo "\n";
echo "JSON: ";
echo json_encode($s), "\n";
var_dump(json_last_error_msg());
foreach ($s as $v) { echo "Unreachable\n"; }

?>
--EXPECTF--
enum(Teds\EmptySet::INSTANCE)
bool(true)
NULL
array(0) {
}
Test iterable methods
NULL
NULL
bool(false)
bool(false)
Caught OutOfBoundsException: Index out of range
Caught Teds\UnsupportedOperationException: Cannot mutate an immutable empty collection
Caught OutOfBoundsException: Index out of range
Caught Error: Trying to clone an uncloneable object of class Teds\EmptySet
%STeds\EmptySet::INSTANCE
JSON: []
string(8) "No error"