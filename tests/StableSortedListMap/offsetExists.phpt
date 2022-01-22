--TEST--
Teds\StableSortedListMap offsetExists
--FILE--
<?php

$it = new Teds\StableSortedListMap(['first' => true, 'second' => null]);
foreach (['first', 'second', 'third'] as $key) {
    try {
        $offsetGet = $it->offsetGet($key);
    } catch (OutOfBoundsException $e) {
        $offsetGet = "Caught {$e->getMessage()}";
    }
    try {
        $getNoDefault = $it->get($key);
    } catch (OutOfBoundsException $e) {
        $getNoDefault = "Caught {$e->getMessage()}";
    }
    $getDefault = $it->get($key, (object)['x' => 'placeholder']);
    printf(
        "%s:\noffsetExists=%s containsKey=%s offsetGet=%s getNoDefault=%s getDefault=%s isset=%s\n\n",
        $key,
        json_encode($it->offsetExists($key)),
        json_encode($it->containsKey($key)),
        json_encode($offsetGet),
        json_encode($getNoDefault),
        json_encode($getDefault),
        json_encode(isset($it[$key]))
    );
}
var_dump($it);
?>
--EXPECT--
first:
offsetExists=true containsKey=true offsetGet=true getNoDefault=true getDefault=true isset=true

second:
offsetExists=false containsKey=true offsetGet=null getNoDefault=null getDefault=null isset=false

third:
offsetExists=false containsKey=false offsetGet="Caught Key not found" getNoDefault="Caught Key not found" getDefault={"x":"placeholder"} isset=false

object(Teds\StableSortedListMap)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    bool(true)
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "second"
    [1]=>
    NULL
  }
}