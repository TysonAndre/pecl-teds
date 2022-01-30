--TEST--
Teds\SortedStrictMap rebalances
--FILE--
<?php
$it = new Teds\SortedStrictMap();
for ($i = 'aa'; $i < 'ak'; $i++) {
    $it[$i] = true;
}
echo json_encode($it->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
var_dump($it);
?>
--EXPECT--
{
    "key": "ad",
    "value": true,
    "red": true,
    "left": {
        "key": "ab",
        "value": true,
        "red": false,
        "left": {
            "key": "aa",
            "value": true,
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": "ac",
            "value": true,
            "red": false,
            "left": [],
            "right": []
        }
    },
    "right": {
        "key": "af",
        "value": true,
        "red": false,
        "left": {
            "key": "ae",
            "value": true,
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": "ah",
            "value": true,
            "red": true,
            "left": {
                "key": "ag",
                "value": true,
                "red": false,
                "left": [],
                "right": []
            },
            "right": {
                "key": "ai",
                "value": true,
                "red": false,
                "left": [],
                "right": {
                    "key": "aj",
                    "value": true,
                    "red": true,
                    "left": [],
                    "right": []
                }
            }
        }
    }
}
object(Teds\SortedStrictMap)#1 (10) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "aa"
    [1]=>
    bool(true)
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "ab"
    [1]=>
    bool(true)
  }
  [2]=>
  array(2) {
    [0]=>
    string(2) "ac"
    [1]=>
    bool(true)
  }
  [3]=>
  array(2) {
    [0]=>
    string(2) "ad"
    [1]=>
    bool(true)
  }
  [4]=>
  array(2) {
    [0]=>
    string(2) "ae"
    [1]=>
    bool(true)
  }
  [5]=>
  array(2) {
    [0]=>
    string(2) "af"
    [1]=>
    bool(true)
  }
  [6]=>
  array(2) {
    [0]=>
    string(2) "ag"
    [1]=>
    bool(true)
  }
  [7]=>
  array(2) {
    [0]=>
    string(2) "ah"
    [1]=>
    bool(true)
  }
  [8]=>
  array(2) {
    [0]=>
    string(2) "ai"
    [1]=>
    bool(true)
  }
  [9]=>
  array(2) {
    [0]=>
    string(2) "aj"
    [1]=>
    bool(true)
  }
}
