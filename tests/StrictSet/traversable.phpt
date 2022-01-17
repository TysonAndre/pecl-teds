--TEST--
Teds\StrictSet constructed from Traversable
--FILE--
<?php

function yields_values() {
    for ($i = 0; $i < 10; $i++) {
        yield "r$i" => "s$i";
    }
    $o = (object)['key' => 'value'];
    yield $o => $o;
    yield 0 => 1;
    yield 0 => 2;
    echo "Done evaluating the generator\n";
}

// Teds\StrictSet eagerly evaluates the passed in Traversable
$it = new Teds\StrictSet(yields_values());
foreach ($it as $value) {
    printf("Value: %s\n", var_export($value, true));
}
echo "Rewind and iterate again starting from r0\n";
foreach ($it as $value) {
    printf("Value: %s\n", var_export($value, true));
}
unset($it);

$emptyIt = new Teds\StrictSet(new ArrayObject());
var_dump($emptyIt);
foreach ($emptyIt as $key => $value) {
    echo "Unreachable\n";
}
foreach ($emptyIt as $key => $value) {
    echo "Unreachable\n";
}
echo "Done\n";


?>
--EXPECT--
Done evaluating the generator
Value: 'r0'
Value: 'r1'
Value: 'r2'
Value: 'r3'
Value: 'r4'
Value: 'r5'
Value: 'r6'
Value: 'r7'
Value: 'r8'
Value: 'r9'
Value: (object) array(
   'key' => 'value',
)
Value: 0
Value: 0
Rewind and iterate again starting from r0
Value: 'r0'
Value: 'r1'
Value: 'r2'
Value: 'r3'
Value: 'r4'
Value: 'r5'
Value: 'r6'
Value: 'r7'
Value: 'r8'
Value: 'r9'
Value: (object) array(
   'key' => 'value',
)
Value: 0
Value: 0
object(Teds\StrictSet)#1 (0) {
}
Done