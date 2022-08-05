--TEST--
Test Teds\EmptyMap
--SKIPIF--
<?php if (PHP_VERSION_ID < 80100) echo "skip enum requires php 8.1+\n"; ?>
--FILE--
<?php
use Teds\EmptyMap;
$map = Teds\EmptyMap::INSTANCE;
function expect_throws(Closure $c) {
    try {
        $c();
        echo "Unexpectedly didn't throw\n";
    } catch (Throwable $t) {
        printf("Caught %s: %s\n", $t::class, $t->getMessage());
    }
}
var_dump($map);
var_dump($map->isEmpty());
var_dump($map->clear());
var_dump($map->toArray());
echo "Test iterable methods\n";
var_dump($map->next());
var_dump($map->rewind());
expect_throws(fn() => $map->current());
expect_throws(fn() => $map->key());
expect_throws(fn() => clone($map));
expect_throws(fn() => $map->get(123));
var_dump($map->get(123, (object)['k' => 'example default for get()']));
foreach ($map as $v) { echo "Unreachable\n"; }
var_export($map);
echo "\n";
echo "JSON: ";
echo json_encode($map), "\n";
var_dump(json_last_error_msg());
var_dump(empty($map[new stdClass()]));
var_dump(isset($map[0]));
var_dump(isset($map[new stdClass()]));

?>
--EXPECTF--
enum(Teds\EmptyMap::INSTANCE)
bool(true)
NULL
array(0) {
}
Test iterable methods
NULL
NULL
Caught OutOfBoundsException: Index out of range
Caught OutOfBoundsException: Index out of range
Caught Error: Trying to clone an uncloneable object of class Teds\EmptyMap
Caught OutOfBoundsException: Key not found
object(stdClass)#%d (1) {
  ["k"]=>
  string(25) "example default for get()"
}
%STeds\EmptyMap::INSTANCE
JSON: []
string(8) "No error"
bool(true)
bool(false)
bool(false)