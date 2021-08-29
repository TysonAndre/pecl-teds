--TEST--
Teds\KeyValueVector push/pop
--FILE--
<?php

function expect_throws(Closure $cb): void {
    try {
        $cb();
        echo "Unexpectedly didn't throw\n";
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
}

echo "Test empty vector\n";
$it = new Teds\KeyValueVector([]);
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
expect_throws(fn() => $it->pop());
expect_throws(fn() => $it->pop());
$it->push(strtoupper('key'), strtoupper('test'));
$it->push(null, ['literal']);
$it->push(1, new stdClass());
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
var_dump($it->pop());
var_dump($it->pop());
echo "After popping 2 elements: ", json_encode($it), "\n";
var_dump($it->pop());
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
$it->shrinkToFit();
printf("count=%d capacity=%d it=%s\n", count($it), $it->capacity(), json_encode($it));
$it->push(new stdClass(), new ArrayObject());
printf("count=%d capacity=%d\n", count($it), $it->capacity());
var_dump($it);

?>
--EXPECT--
Test empty vector
count=0 capacity=0
Caught UnderflowException: Cannot pop from empty Teds\KeyValueVector
Caught UnderflowException: Cannot pop from empty Teds\KeyValueVector
[["KEY","TEST"],[null,["literal"]],[1,{}]]
count=3 capacity=4
array(2) {
  [0]=>
  int(1)
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  NULL
  [1]=>
  array(1) {
    [0]=>
    string(7) "literal"
  }
}
After popping 2 elements: [["KEY","TEST"]]
array(2) {
  [0]=>
  string(3) "KEY"
  [1]=>
  string(4) "TEST"
}
[]
count=0 capacity=4
count=0 capacity=0 it=[]
count=1 capacity=4
object(Teds\KeyValueVector)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    object(ArrayObject)#3 (1) {
      ["storage":"ArrayObject":private]=>
      array(0) {
      }
    }
  }
}