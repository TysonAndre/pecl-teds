--TEST--
Test Teds\EmptySequence init()
--SKIPIF--
<?php if (PHP_VERSION_ID < 80100) echo "skip requires php 8.1+ enums\n"; ?>
--FILE--
<?php
use Teds\EmptySequence;
$s = Teds\EmptySequence::INSTANCE;
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
expect_throws(fn() => $s->current());
expect_throws(fn() => $s->key());
expect_throws(fn() => clone($s));
foreach ($s as $v) { echo "Unreachable\n"; }
var_export($s);
echo "\n";
echo "JSON: ";
echo json_encode($s), "\n";
var_dump(json_last_error_msg());
var_dump($s->map(fn() => print("map not called\n")));
var_dump($s->filter(fn() => print("filter not called\n")));

?>
--EXPECTF--
enum(Teds\EmptySequence::INSTANCE)
bool(true)
NULL
array(0) {
}
Test iterable methods
NULL
NULL
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught Error: Trying to clone an uncloneable object of class Teds\EmptySequence
%STeds\EmptySequence::INSTANCE
JSON: []
string(8) "No error"
enum(Teds\EmptySequence::INSTANCE)
enum(Teds\EmptySequence::INSTANCE)
