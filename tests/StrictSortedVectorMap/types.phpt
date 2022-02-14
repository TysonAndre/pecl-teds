--TEST--
Teds\StrictSortedVectorMap supports various types
--FILE--
<?php

function create_key(int $i) {
    return "k$i";
}
$it = new Teds\StrictSortedVectorMap();
$values = [
    0,
    '0',
    null,
    false,
    true,
    0.0,
    1.0,
    0.5,
    1,
    INF,
    NAN,
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
object(Teds\StrictSortedVectorMap)#1 (14) {
  [0]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    string(2) "k2"
  }
  [1]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    string(2) "k3"
  }
  [2]=>
  array(2) {
    [0]=>
    bool(true)
    [1]=>
    string(2) "k4"
  }
  [3]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(2) "k0"
  }
  [4]=>
  array(2) {
    [0]=>
    float(0)
    [1]=>
    string(2) "k5"
  }
  [5]=>
  array(2) {
    [0]=>
    float(0.5)
    [1]=>
    string(2) "k7"
  }
  [6]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(2) "k8"
  }
  [7]=>
  array(2) {
    [0]=>
    float(1)
    [1]=>
    string(2) "k6"
  }
  [8]=>
  array(2) {
    [0]=>
    float(INF)
    [1]=>
    string(2) "k9"
  }
  [9]=>
  array(2) {
    [0]=>
    float(NAN)
    [1]=>
    string(3) "k10"
  }
  [10]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(2) "k1"
  }
  [11]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(2) "k0"
    }
    [1]=>
    string(3) "k11"
  }
  [12]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    string(3) "k12"
  }
  [13]=>
  array(2) {
    [0]=>
    resource(1) of type (stream)
    [1]=>
    string(3) "k13"
  }
}
object(Teds\StrictSortedVectorMap)#1 (14) {
  [0]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    string(4) "k102"
  }
  [1]=>
  array(2) {
    [0]=>
    bool(false)
    [1]=>
    string(4) "k103"
  }
  [2]=>
  array(2) {
    [0]=>
    bool(true)
    [1]=>
    string(4) "k104"
  }
  [3]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(4) "k100"
  }
  [4]=>
  array(2) {
    [0]=>
    float(0)
    [1]=>
    string(4) "k105"
  }
  [5]=>
  array(2) {
    [0]=>
    float(0.5)
    [1]=>
    string(4) "k107"
  }
  [6]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(4) "k108"
  }
  [7]=>
  array(2) {
    [0]=>
    float(1)
    [1]=>
    string(4) "k106"
  }
  [8]=>
  array(2) {
    [0]=>
    float(INF)
    [1]=>
    string(4) "k109"
  }
  [9]=>
  array(2) {
    [0]=>
    float(NAN)
    [1]=>
    string(4) "k110"
  }
  [10]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(4) "k101"
  }
  [11]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(2) "k0"
    }
    [1]=>
    string(4) "k111"
  }
  [12]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    string(4) "k112"
  }
  [13]=>
  array(2) {
    [0]=>
    resource(1) of type (stream)
    [1]=>
    string(4) "k113"
  }
}
After unsetting, count=0
object(Teds\StrictSortedVectorMap)#1 (0) {
}