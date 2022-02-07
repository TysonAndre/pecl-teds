--TEST--
Teds\IntVector gc
--FILE--
<?php
call_user_func(function () {
    $vec = new Teds\IntVector([123]);
    $vec2 = new Teds\IntVector([456]);
    gc_collect_cycles();
    var_dump($vec, $vec2);
    echo var_export($vec, true), "\n";
    gc_collect_cycles();
});
call_user_func(function () {
    $it = (new Teds\IntVector([123]))->getIterator();
    gc_collect_cycles();
    var_dump($it);
});
?>
--EXPECT--
object(Teds\IntVector)#2 (1) {
  [0]=>
  int(123)
}
object(Teds\IntVector)#3 (1) {
  [0]=>
  int(456)
}
Teds\IntVector::__set_state(array(
   0 => 123,
))
object(InternalIterator)#4 (0) {
}