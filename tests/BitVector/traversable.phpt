--TEST--
Teds\BitVector constructed from Traversable
--FILE--
<?php

function yields_values() {
    for ($i = 0; $i < 70; $i++) {
        yield "r$i" => $i % 3 === 0;
    }
}

// Teds\BitVector eagerly evaluates the passed in Traversable
$it = new Teds\BitVector(yields_values());
echo json_encode($it), "\n";
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
echo json_encode(iterator_to_array($it)), "\n";
echo "Rewind and iterate again starting from r0\n";
echo json_encode(iterator_to_array($it)), "\n";
unset($it);

$emptyIt = new Teds\BitVector(new ArrayObject());
var_dump($emptyIt);
foreach ($emptyIt as $key => $value) {
    echo "Unreachable\n";
}
foreach ($emptyIt as $key => $value) {
    echo "Unreachable\n";
}
echo "Done\n";
unset($emptyIt);

?>
--EXPECT--
[true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true]
count=70 capacity=128
[true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true]
Rewind and iterate again starting from r0
[true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true,false,false,true]
object(Teds\BitVector)#1 (0) {
}
Done