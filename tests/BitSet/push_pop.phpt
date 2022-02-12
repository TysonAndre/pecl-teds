--TEST--
Teds\BitSet push/pop
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

echo "Test empty BitSet\n";
$it = new Teds\BitSet([]);
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
expect_throws(fn() => $it->pop());
expect_throws(fn() => $it->pop());
$it->push(true);
$it->push(true);
$it[] = false;
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
var_dump($it->pop());
var_dump($it->pop());
echo "After popping 2 elements: ", json_encode($it->toArray()), "\n";
var_dump($it->pop());
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
echo "After pushing variadic args\n";
$it->push(true, false, true, false, true, true, true, false);
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
$it->pushBits(true, false, true, false);
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());

?>
--EXPECT--
Test empty BitSet
count=0 capacity=0
Caught UnderflowException: Cannot pop from empty Teds\BitSet
Caught UnderflowException: Cannot pop from empty Teds\BitSet
[true,true,false]
count=3 capacity=64
bool(false)
bool(true)
After popping 2 elements: [true]
bool(true)
[]
count=0 capacity=64
After pushing variadic args
[true,false,true,false,true,true,true,false]
count=8 capacity=64
[true,false,true,false,true,true,true,false,true,false,true,false]
count=12 capacity=64