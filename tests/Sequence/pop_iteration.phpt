--TEST--
Teds\Sequence pop() with iterators
--FILE--
<?php
function print_it_state(Iterator $it, string $name) {
    if (!$it->valid()) {
        printf("%s valid=false\n", $name);
    } else {
        printf("%s valid=true key=%s value=%s\n", $name, json_encode($it->key()), json_encode($it->current()));
    }
}
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
    Teds\IntVector::class,
    Teds\BitVector::class,
] as $class_name) {
    echo "Test $class_name\n";
    if ($class_name !== Teds\BitVector::class) {
        $sequence = new $class_name([1000]);
    } else {
        $sequence = new $class_name([true]);
    }
    $it1 = $sequence->getIterator();
    $it2 = $sequence->getIterator();
    $it2->next();
    print_it_state($it1, 'it1');
    print_it_state($it2, 'it2');
    $v = $sequence->pop();
    echo "Popped: ";
    var_dump($v);
    print_it_state($it1, 'it1');
    print_it_state($it2, 'it2');
    echo "After push\n";
    $sequence->push($v);
    print_it_state($it1, 'it1');
    print_it_state($it2, 'it2');
    $it1->next();
    print_it_state($it1, 'it1 after next()');
    echo "After push again\n";
    $sequence->push(is_int($v) ? $v * 2 : !$v);
    print_it_state($it1, 'it1');
    print_it_state($it2, 'it2');
}
?>
--EXPECT--
Test Teds\Deque
it1 valid=true key=0 value=1000
it2 valid=false
Popped: int(1000)
it1 valid=false
it2 valid=false
After push
it1 valid=false
it2 valid=false
it1 after next() valid=true key=0 value=1000
After push again
it1 valid=true key=0 value=1000
it2 valid=true key=1 value=2000
Test Teds\Vector
it1 valid=true key=0 value=1000
it2 valid=false
Popped: int(1000)
it1 valid=false
it2 valid=false
After push
it1 valid=false
it2 valid=false
it1 after next() valid=true key=0 value=1000
After push again
it1 valid=true key=0 value=1000
it2 valid=true key=1 value=2000
Test Teds\LowMemoryVector
it1 valid=true key=0 value=1000
it2 valid=false
Popped: int(1000)
it1 valid=false
it2 valid=false
After push
it1 valid=false
it2 valid=false
it1 after next() valid=true key=0 value=1000
After push again
it1 valid=true key=0 value=1000
it2 valid=true key=1 value=2000
Test Teds\IntVector
it1 valid=true key=0 value=1000
it2 valid=false
Popped: int(1000)
it1 valid=false
it2 valid=false
After push
it1 valid=false
it2 valid=false
it1 after next() valid=true key=0 value=1000
After push again
it1 valid=true key=0 value=1000
it2 valid=true key=1 value=2000
Test Teds\BitVector
it1 valid=true key=0 value=true
it2 valid=false
Popped: bool(true)
it1 valid=false
it2 valid=false
After push
it1 valid=false
it2 valid=false
it1 after next() valid=true key=0 value=true
After push again
it1 valid=true key=0 value=true
it2 valid=true key=1 value=false