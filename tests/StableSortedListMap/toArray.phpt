--TEST--
Teds\StableSortedListMap toArray()
--FILE--
<?php
function create_dynamic(string $key) { return "v_$key"; }
call_user_func(function () {
    $it = new Teds\StableSortedListMap([create_dynamic('abc') => create_dynamic('x'), 'def' => new stdClass()]);
    var_dump($it->toArray());
});
call_user_func(function () {
    $it = new Teds\StableSortedListMap();
    $it[[]] = 123;
    try {
        var_dump($it->toArray());
    } catch (TypeError $e) {
        echo "Caught: {$e->getMessage()}\n";
    }
});

?>
--EXPECT--
array(2) {
  ["def"]=>
  object(stdClass)#3 (0) {
  }
  ["v_abc"]=>
  string(3) "v_x"
}
Caught: Illegal offset type