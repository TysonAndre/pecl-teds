--TEST--
Teds\IntVector constructed from Traversable
--FILE--
<?php

function yields_values() {
    for ($i = 0; $i < 10; $i++) {
        yield "r$i" => 1 << $i ;
    }
    $o = new ArrayObject();
    yield $o => 123;
    yield 0 => 1;
    yield 0 => 2;
    echo "Done evaluating the generator\n";
}

// Teds\IntVector eagerly evaluates the passed in Traversable
$it = new Teds\IntVector(yields_values());
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
echo "Rewind and iterate again starting from r0\n";
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
unset($it);

$emptyIt = new Teds\IntVector(new ArrayObject());
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
Done evaluating the generator
Key: 0
Value: 1
Key: 1
Value: 2
Key: 2
Value: 4
Key: 3
Value: 8
Key: 4
Value: 16
Key: 5
Value: 32
Key: 6
Value: 64
Key: 7
Value: 128
Key: 8
Value: 256
Key: 9
Value: 512
Key: 10
Value: 123
Key: 11
Value: 1
Key: 12
Value: 2
Rewind and iterate again starting from r0
Key: 0
Value: 1
Key: 1
Value: 2
Key: 2
Value: 4
Key: 3
Value: 8
Key: 4
Value: 16
Key: 5
Value: 32
Key: 6
Value: 64
Key: 7
Value: 128
Key: 8
Value: 256
Key: 9
Value: 512
Key: 10
Value: 123
Key: 11
Value: 1
Key: 12
Value: 2
object(Teds\IntVector)#1 (0) {
}
Done