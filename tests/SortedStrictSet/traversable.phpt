--TEST--
Teds\SortedStrictSet constructed from Traversable
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

// Teds\SortedStrictSet eagerly evaluates the passed in Traversable
$it = new Teds\SortedStrictSet(yields_values());
foreach ($it as $value) {
    printf("Value: %s\n", var_export($value, true));
}
echo "Rewind and iterate again starting from r0\n";
foreach ($it as $value) {
    printf("Value: %s\n", var_export($value, true));
}
unset($it);

$emptyIt = new Teds\SortedStrictSet(new ArrayObject());
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
Value: 1
Value: 2
Value: 's0'
Value: 's1'
Value: 's2'
Value: 's3'
Value: 's4'
Value: 's5'
Value: 's6'
Value: 's7'
Value: 's8'
Value: 's9'
Value: (object) array(
   'key' => 'value',
)
Rewind and iterate again starting from r0
Value: 1
Value: 2
Value: 's0'
Value: 's1'
Value: 's2'
Value: 's3'
Value: 's4'
Value: 's5'
Value: 's6'
Value: 's7'
Value: 's8'
Value: 's9'
Value: (object) array(
   'key' => 'value',
)
object(Teds\SortedStrictSet)#1 (0) {
}
Done