--TEST--
Teds\StrictMap supports various types
--FILE--
<?php

function create_key(int $i) {
    return "k$i";
}
$it = new Teds\StrictMap();
$values = [
    0,
    '0',
    null,
    false,
    true,
    0.0,
    1.0,
    INF,
    [create_key(0)],
    new stdClass(), // different instance
    STDIN
];
foreach ($values as $i => $value) {
    $it[$value] = create_key($i);
}

var_dump($it);
foreach ($values as $i => $value) {
    $it[$value] = create_key(100 + $i);
}
var_dump($it);
foreach ($values as $i => $value) {
    unset($it[$value]);
}
printf("After unsetting, count=%d\n", count($it));
var_dump($it);

?>
--EXPECT--
object(Teds\StrictMap)#1 (11) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(2) "k0"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(2) "k1"
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    string(2) "k2"
  }
  [3]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    string(2) "k3"
  }
  [4]=>
  array(2) {
    [0]=>
    bool(true)
    [1]=>
    string(2) "k4"
  }
  [5]=>
  array(2) {
    [0]=>
    float(0)
    [1]=>
    string(2) "k5"
  }
  [6]=>
  array(2) {
    [0]=>
    float(1)
    [1]=>
    string(2) "k6"
  }
  [7]=>
  array(2) {
    [0]=>
    float(INF)
    [1]=>
    string(2) "k7"
  }
  [8]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(2) "k0"
    }
    [1]=>
    string(2) "k8"
  }
  [9]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    string(2) "k9"
  }
  [10]=>
  array(2) {
    [0]=>
    resource(1) of type (stream)
    [1]=>
    string(3) "k10"
  }
}
object(Teds\StrictMap)#1 (11) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(4) "k100"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(4) "k101"
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    string(4) "k102"
  }
  [3]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    string(4) "k103"
  }
  [4]=>
  array(2) {
    [0]=>
    bool(true)
    [1]=>
    string(4) "k104"
  }
  [5]=>
  array(2) {
    [0]=>
    float(0)
    [1]=>
    string(4) "k105"
  }
  [6]=>
  array(2) {
    [0]=>
    float(1)
    [1]=>
    string(4) "k106"
  }
  [7]=>
  array(2) {
    [0]=>
    float(INF)
    [1]=>
    string(4) "k107"
  }
  [8]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(2) "k0"
    }
    [1]=>
    string(4) "k108"
  }
  [9]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    string(4) "k109"
  }
  [10]=>
  array(2) {
    [0]=>
    resource(1) of type (stream)
    [1]=>
    string(4) "k110"
  }
}
After unsetting, count=0
object(Teds\StrictMap)#1 (0) {
}