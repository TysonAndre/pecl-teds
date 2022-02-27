--TEST--
Teds\Sequence::__set_state
--FILE--
<?php

function dump_repr($obj) {
    echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
}
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
    Teds\ImmutableSequence::class,
] as $class_name) {
    echo "Test $class_name\n";
    dump_repr($class_name::__set_state([]));
    $it = $class_name::__set_state([strtoupper('a literal'), ['first', 'x'], [(object)['key' => 'value'], null]]);
    foreach ($it as $key => $value) {
        printf("key=%s value=%s\n", json_encode($key), json_encode($value));
    }
    dump_repr($it);
    var_dump($it);
    var_dump((array)$it);
}

?>
--EXPECTF--
Test Teds\Deque
Teds\Deque::__set_state(array(
))
key=0 value="A LITERAL"
key=1 value=["first","x"]
key=2 value=[{"key":"value"},null]
Teds\Deque::__set_state(array(
   0 => 'A LITERAL',
   1 =>
  array (
    0 => 'first',
    1 => 'x',
  ),
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Teds\Deque)#%d (3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
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
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
Test Teds\Vector
Teds\Vector::__set_state(array(
))
key=0 value="A LITERAL"
key=1 value=["first","x"]
key=2 value=[{"key":"value"},null]
Teds\Vector::__set_state(array(
   0 => 'A LITERAL',
   1 =>
  array (
    0 => 'first',
    1 => 'x',
  ),
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Teds\Vector)#%d (3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
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
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
Test Teds\LowMemoryVector
Teds\LowMemoryVector::__set_state(array(
))
key=0 value="A LITERAL"
key=1 value=["first","x"]
key=2 value=[{"key":"value"},null]
Teds\LowMemoryVector::__set_state(array(
))
object(Teds\LowMemoryVector)#%d (0) {
}
array(3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}
Test Teds\ImmutableSequence
Teds\ImmutableSequence::__set_state(array(
))
key=0 value="A LITERAL"
key=1 value=["first","x"]
key=2 value=[{"key":"value"},null]
Teds\ImmutableSequence::__set_state(array(
   0 => 'A LITERAL',
   1 =>
  array (
    0 => 'first',
    1 => 'x',
  ),
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Teds\ImmutableSequence)#%d (3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
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
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#%d (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}