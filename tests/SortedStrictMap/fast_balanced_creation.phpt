--TEST--
Teds\SortedStrictMap balanced created from array
--FILE--
<?php

function test_count(int $n) {
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[] = "v$i";
    }
    echo "Test n=$n\n";
    $it = new Teds\SortedStrictMap($values);
    echo json_encode($it->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
    $it2 = unserialize(serialize($it));
    echo json_encode($it2->debugGetTreeRepresentation(), JSON_PRETTY_PRINT), "\n";
    echo json_encode($it2), "\n";
}
test_count(1);
test_count(2);
test_count(3);
test_count(4);
test_count(7);
?>
--EXPECT--
Test n=1
{
    "key": 0,
    "value": "v0",
    "red": false,
    "left": [],
    "right": []
}
{
    "key": 0,
    "value": "v0",
    "red": false,
    "left": [],
    "right": []
}
[[0,"v0"]]
Test n=2
{
    "key": 1,
    "value": "v1",
    "red": false,
    "left": {
        "key": 0,
        "value": "v0",
        "red": false,
        "left": [],
        "right": []
    },
    "right": []
}
{
    "key": 1,
    "value": "v1",
    "red": false,
    "left": {
        "key": 0,
        "value": "v0",
        "red": false,
        "left": [],
        "right": []
    },
    "right": []
}
[[0,"v0"],[1,"v1"]]
Test n=3
{
    "key": 1,
    "value": "v1",
    "red": false,
    "left": {
        "key": 0,
        "value": "v0",
        "red": false,
        "left": [],
        "right": []
    },
    "right": {
        "key": 2,
        "value": "v2",
        "red": false,
        "left": [],
        "right": []
    }
}
{
    "key": 1,
    "value": "v1",
    "red": false,
    "left": {
        "key": 0,
        "value": "v0",
        "red": false,
        "left": [],
        "right": []
    },
    "right": {
        "key": 2,
        "value": "v2",
        "red": false,
        "left": [],
        "right": []
    }
}
[[0,"v0"],[1,"v1"],[2,"v2"]]
Test n=4
{
    "key": 2,
    "value": "v2",
    "red": false,
    "left": {
        "key": 1,
        "value": "v1",
        "red": false,
        "left": {
            "key": 0,
            "value": "v0",
            "red": true,
            "left": [],
            "right": []
        },
        "right": []
    },
    "right": {
        "key": 3,
        "value": "v3",
        "red": false,
        "left": [],
        "right": []
    }
}
{
    "key": 2,
    "value": "v2",
    "red": false,
    "left": {
        "key": 1,
        "value": "v1",
        "red": false,
        "left": {
            "key": 0,
            "value": "v0",
            "red": true,
            "left": [],
            "right": []
        },
        "right": []
    },
    "right": {
        "key": 3,
        "value": "v3",
        "red": false,
        "left": [],
        "right": []
    }
}
[[0,"v0"],[1,"v1"],[2,"v2"],[3,"v3"]]
Test n=7
{
    "key": 3,
    "value": "v3",
    "red": false,
    "left": {
        "key": 1,
        "value": "v1",
        "red": false,
        "left": {
            "key": 0,
            "value": "v0",
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": 2,
            "value": "v2",
            "red": false,
            "left": [],
            "right": []
        }
    },
    "right": {
        "key": 5,
        "value": "v5",
        "red": false,
        "left": {
            "key": 4,
            "value": "v4",
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": 6,
            "value": "v6",
            "red": false,
            "left": [],
            "right": []
        }
    }
}
{
    "key": 3,
    "value": "v3",
    "red": false,
    "left": {
        "key": 1,
        "value": "v1",
        "red": false,
        "left": {
            "key": 0,
            "value": "v0",
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": 2,
            "value": "v2",
            "red": false,
            "left": [],
            "right": []
        }
    },
    "right": {
        "key": 5,
        "value": "v5",
        "red": false,
        "left": {
            "key": 4,
            "value": "v4",
            "red": false,
            "left": [],
            "right": []
        },
        "right": {
            "key": 6,
            "value": "v6",
            "red": false,
            "left": [],
            "right": []
        }
    }
}
[[0,"v0"],[1,"v1"],[2,"v2"],[3,"v3"],[4,"v4"],[5,"v5"],[6,"v6"]]