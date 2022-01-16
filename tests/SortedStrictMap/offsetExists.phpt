--TEST--
Teds\SortedStrictMap offsetExists
--FILE--
<?php

$it = new Teds\SortedStrictMap(['first' => true, 'second' => null]);
foreach (['first', 'second', 'third'] as $key) {
    printf(
        "%s:\noffsetExists=%s containsKey=%s offsetGet=%s isset=%s\n\n",
        $key,
        json_encode($it->offsetExists($key)),
        json_encode($it->containsKey($key)),
        json_encode($it->offsetGet($key)),
        json_encode(isset($it[$key]))
    );
}
?>
--EXPECT--
first:
offsetExists=true containsKey=true offsetGet=true isset=true

second:
offsetExists=false containsKey=true offsetGet=null isset=false

third:
offsetExists=false containsKey=false offsetGet=null isset=false