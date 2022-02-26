--TEST--
Teds\BitVector gc
--FILE--
<?php
call_user_func(function () {
    $vec = new Teds\BitVector([true, false, true, true, true, true, false, false, true, false, true]);
    echo json_encode($vec), "\n";
    $vec2 = new Teds\BitVector([true]);
    gc_collect_cycles();
    var_dump($vec, $vec2);
    echo var_export($vec, true), "\n";
    gc_collect_cycles();
});
call_user_func(function () {
    $it = (new Teds\BitVector([false]))->getIterator();
    gc_collect_cycles();
    var_dump($it);
    var_dump($it->current());
    var_dump($it->key());
});
?>
--EXPECT--
[true,false,true,true,true,true,false,false,true,false,true]
object(Teds\BitVector)#2 (11) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
  [4]=>
  bool(true)
  [5]=>
  bool(true)
  [6]=>
  bool(false)
  [7]=>
  bool(false)
  [8]=>
  bool(true)
  [9]=>
  bool(false)
  [10]=>
  bool(true)
}
object(Teds\BitVector)#3 (1) {
  [0]=>
  bool(true)
}
Teds\BitVector::__set_state(array(
   0 => true,
   1 => false,
   2 => true,
   3 => true,
   4 => true,
   5 => true,
   6 => false,
   7 => false,
   8 => true,
   9 => false,
   10 => true,
))
object(InternalIterator)#4 (0) {
}
bool(false)
int(0)