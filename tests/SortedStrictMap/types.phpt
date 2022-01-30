--TEST--
Teds\SortedStrictMap supports various types
--FILE--
<?php

function create_key(int $i) {
    return "k$i";
}
$it = new Teds\SortedStrictMap();
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
echo "Tree representation\n";
var_dump($it->debugGetTreeRepresentation());
var_dump($it);
foreach ($values as $value) {
    echo "Remove: "; var_dump($value);
    unset($it[$value]);
}
printf("After unsetting, count=%d\n", count($it));
var_dump($it);

?>
--EXPECT--
object(Teds\SortedStrictMap)#1 (14) {
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
Tree representation
array(5) {
  ["key"]=>
  float(1)
  ["value"]=>
  string(4) "k106"
  ["red"]=>
  bool(true)
  ["left"]=>
  array(5) {
    ["key"]=>
    int(0)
    ["value"]=>
    string(4) "k100"
    ["red"]=>
    bool(false)
    ["left"]=>
    array(5) {
      ["key"]=>
      bool(false)
      ["value"]=>
      string(4) "k103"
      ["red"]=>
      bool(false)
      ["left"]=>
      array(5) {
        ["key"]=>
        NULL
        ["value"]=>
        string(4) "k102"
        ["red"]=>
        bool(true)
        ["left"]=>
        array(0) {
        }
        ["right"]=>
        array(0) {
        }
      }
      ["right"]=>
      array(5) {
        ["key"]=>
        bool(true)
        ["value"]=>
        string(4) "k104"
        ["red"]=>
        bool(true)
        ["left"]=>
        array(0) {
        }
        ["right"]=>
        array(0) {
        }
      }
    }
    ["right"]=>
    array(5) {
      ["key"]=>
      float(0.5)
      ["value"]=>
      string(4) "k107"
      ["red"]=>
      bool(false)
      ["left"]=>
      array(5) {
        ["key"]=>
        float(0)
        ["value"]=>
        string(4) "k105"
        ["red"]=>
        bool(true)
        ["left"]=>
        array(0) {
        }
        ["right"]=>
        array(0) {
        }
      }
      ["right"]=>
      array(5) {
        ["key"]=>
        int(1)
        ["value"]=>
        string(4) "k108"
        ["red"]=>
        bool(true)
        ["left"]=>
        array(0) {
        }
        ["right"]=>
        array(0) {
        }
      }
    }
  }
  ["right"]=>
  array(5) {
    ["key"]=>
    float(NAN)
    ["value"]=>
    string(4) "k110"
    ["red"]=>
    bool(false)
    ["left"]=>
    array(5) {
      ["key"]=>
      float(INF)
      ["value"]=>
      string(4) "k109"
      ["red"]=>
      bool(false)
      ["left"]=>
      array(0) {
      }
      ["right"]=>
      array(0) {
      }
    }
    ["right"]=>
    array(5) {
      ["key"]=>
      array(1) {
        [0]=>
        string(2) "k0"
      }
      ["value"]=>
      string(4) "k111"
      ["red"]=>
      bool(true)
      ["left"]=>
      array(5) {
        ["key"]=>
        string(1) "0"
        ["value"]=>
        string(4) "k101"
        ["red"]=>
        bool(false)
        ["left"]=>
        array(0) {
        }
        ["right"]=>
        array(0) {
        }
      }
      ["right"]=>
      array(5) {
        ["key"]=>
        object(stdClass)#2 (0) {
        }
        ["value"]=>
        string(4) "k112"
        ["red"]=>
        bool(false)
        ["left"]=>
        array(0) {
        }
        ["right"]=>
        array(5) {
          ["key"]=>
          resource(1) of type (stream)
          ["value"]=>
          string(4) "k113"
          ["red"]=>
          bool(true)
          ["left"]=>
          array(0) {
          }
          ["right"]=>
          array(0) {
          }
        }
      }
    }
  }
}
object(Teds\SortedStrictMap)#1 (14) {
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
Remove: int(0)
Remove: string(1) "0"
Remove: NULL
Remove: bool(false)
Remove: bool(true)
Remove: float(0)
Remove: float(1)
Remove: float(0.5)
Remove: int(1)
Remove: float(INF)
Remove: float(NAN)
Remove: array(1) {
  [0]=>
  string(2) "k0"
}
Remove: object(stdClass)#2 (0) {
}
Remove: resource(1) of type (stream)
After unsetting, count=0
object(Teds\SortedStrictMap)#1 (0) {
}