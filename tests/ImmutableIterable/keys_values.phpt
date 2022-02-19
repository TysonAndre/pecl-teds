--TEST--
Teds\ImmutableIterable keys() and values()
--FILE--
<?php

foreach ([Teds\ImmutableIterable::class, Teds\CachedIterable::class] as $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(['first' => new stdClass()]);
    var_dump($it->keys());
    var_dump($it->values());
    $it = new $class_name([]);
    var_dump($it->keys());
    var_dump($it->values());
}
?>
--EXPECT--
Test Teds\ImmutableIterable
array(1) {
  [0]=>
  string(5) "first"
}
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(0) {
}
array(0) {
}
Test Teds\CachedIterable
array(1) {
  [0]=>
  string(5) "first"
}
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(0) {
}
array(0) {
}