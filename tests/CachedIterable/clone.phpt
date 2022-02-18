--TEST--
Teds\CachedIterable can be cloned
--FILE--
<?php

$it = Teds\CachedIterable::fromPairs([[new stdClass(), new ArrayObject()]]);
try {
    clone $it;
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
var_dump($it);

?>
--EXPECT--
Caught Error: Trying to clone an uncloneable object of class Teds\CachedIterable
object(Teds\CachedIterable)#3 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (0) {
    }
    [1]=>
    object(ArrayObject)#2 (1) {
      ["storage":"ArrayObject":private]=>
      array(0) {
      }
    }
  }
}