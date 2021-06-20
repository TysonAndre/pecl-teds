--TEST--
Teds\Deque pushBack/popBack
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

echo "Test empty deque\n";
$it = new Teds\Deque([]);
expect_throws(fn() => $it->popBack());
expect_throws(fn() => $it->popBack());
$it->pushBack(strtoupper('test'));
$it->pushBack(['literal']);
$it->pushBack(new stdClass());
echo json_encode($it), "\n";
printf("count=%d\n", count($it));
var_dump($it->popBack());
var_dump($it->popBack());
echo "After popping 2 elements: ", json_encode($it->toArray()), "\n";
var_dump($it->popBack());
echo json_encode($it), "\n";
printf("count=%d\n", count($it));

?>
--EXPECT--
Test empty deque
Caught UnderflowException: Cannot popBack from empty deque
Caught UnderflowException: Cannot popBack from empty deque
["TEST",["literal"],{}]
count=3
object(stdClass)#2 (0) {
}
array(1) {
  [0]=>
  string(7) "literal"
}
After popping 2 elements: ["TEST"]
string(4) "TEST"
[]
count=0