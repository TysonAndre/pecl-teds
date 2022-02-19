--TEST--
Teds\CachedIterable Lazy evaluation
--FILE--
<?php
function yield_values() {
    echo "Started yield_values call\n";
    for ($i = 0; $i < 3; $i++) {
        echo "Yielding i=$i\n";
        yield "a$i" => "b$i";
    }
}

$iterable = new Teds\CachedIterable(yield_values());
echo "Created iterable\n";
foreach ($iterable as $k1 => $v1) {
    echo "k1=$k1 v1=$v1\n";
    foreach ($iterable as $k2 => $v2) {
        echo "- k2=$k2 v2=$v2\n";
    }
}

--EXPECT--
Started yield_values call
Yielding i=0
Created iterable
k1=a0 v1=b0
- k2=a0 v2=b0
Yielding i=1
- k2=a1 v2=b1
Yielding i=2
- k2=a2 v2=b2
k1=a1 v1=b1
- k2=a0 v2=b0
- k2=a1 v2=b1
- k2=a2 v2=b2
k1=a2 v1=b2
- k2=a0 v2=b0
- k2=a1 v2=b1
- k2=a2 v2=b2