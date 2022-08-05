--TEST--
Teds\StrictHashSet::__set_state
--FILE--
<?php

function dump_repr($obj) {
    echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
}

function test_set_impl_set_state(string $class_name) {
    echo "Test $class_name\n";
    dump_repr($class_name::__set_state([]));
    $it = $class_name::__set_state([strtoupper('a literal'), null, [(object)['key' => 'value'], null]]);
    foreach ($it as $value) {
        printf("- %s\n", json_encode($value));
    }
    dump_repr($it);
    var_dump($it);
    var_dump((array)$it);
}
foreach ([
    Teds\StrictTreeSet::class,
    Teds\StrictHashSet::class,
    Teds\StrictSortedVectorSet::class,
] as $class_name) {
    test_set_impl_set_state($class_name);
}

?>
--EXPECTF--
Test Teds\StrictTreeSet
%STeds\StrictTreeSet::__set_state(array(
))
- null
- "A LITERAL"
- [{"key":"value"},null]
%STeds\StrictTreeSet::__set_state(array(
   0 => NULL,
   1 => 'A LITERAL',
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Teds\StrictTreeSet)#2 (3) {
  [0]=>
  NULL
  [1]=>
  string(9) "A LITERAL"
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
array(3) {
  [0]=>
  NULL
  [1]=>
  string(9) "A LITERAL"
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
Test Teds\StrictHashSet
%STeds\StrictHashSet::__set_state(array(
))
- "A LITERAL"
- null
- [{"key":"value"},null]
%STeds\StrictHashSet::__set_state(array(
   0 => 'A LITERAL',
   1 => NULL,
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Teds\StrictHashSet)#2 (3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  NULL
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
array(3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  NULL
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
Test Teds\StrictSortedVectorSet
%STeds\StrictSortedVectorSet::__set_state(array(
))
- null
- "A LITERAL"
- [{"key":"value"},null]
%STeds\StrictSortedVectorSet::__set_state(array(
   0 => NULL,
   1 => 'A LITERAL',
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Teds\StrictSortedVectorSet)#2 (3) {
  [0]=>
  NULL
  [1]=>
  string(9) "A LITERAL"
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
array(3) {
  [0]=>
  NULL
  [1]=>
  string(9) "A LITERAL"
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}