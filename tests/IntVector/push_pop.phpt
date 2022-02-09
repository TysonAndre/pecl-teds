--TEST--
Teds\IntVector push/pop
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
$it = new Teds\IntVector([]);
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
expect_throws(fn() => $it->pop());
expect_throws(fn() => $it->pop());
$it->push(1_999_999_999);
$it->push(-1_999_999_999);
$it[] = 123;
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
var_dump($it->pop());
var_dump($it->pop());
echo "After popping 2 elements: ", json_encode($it->toArray()), "\n";
var_dump($it->pop());
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
echo "After pushing variadic args\n";
$it->push(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());
$it->pushInts(1, -1, 1, 567, 123, 11, 11);
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", count($it), $it->capacity());

?>
--EXPECT--
Test empty vector
count=0 capacity=0
Caught UnderflowException: Cannot pop from empty Teds\IntVector
Caught UnderflowException: Cannot pop from empty Teds\IntVector
[1999999999,-1999999999,123]
count=3 capacity=4
int(123)
int(-1999999999)
After popping 2 elements: [1999999999]
int(1999999999)
[]
count=0 capacity=4
After pushing variadic args
[1,2,3,4,5,6,7,8,9,10]
count=10 capacity=16
[1,2,3,4,5,6,7,8,9,10,1,-1,1,567,123,11,11]
count=17 capacity=32