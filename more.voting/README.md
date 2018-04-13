# voting contract
## build and deploy
```shell
$ eosiocpp -o contracts/more.voting/more.voting.wast contracts/more.voting/more.voting.cpp
$ cleos set contract more.voting contracts/more.voting -p more.voting
```
## run
### create a voting

Edit file `create.json`

```json
{
    "creator": "more",
    "vname": "voting",
    "proposals": [{
        "pname": "first",
        "description": "the first proposal"
    }, {
        "pname": "second",
        "description": "the second proposal"
    }],
    "expiration": "2018-04-13T10:08:30"
}
```

Run the command

```shell
$ cleos push action more.voting create "$(cat voting/create.json)" -p more
```
Then query the `vrecord` table to view the added voting

```shell
$ cleos get table more.voting more vrecord
{
  "rows": [{
      "vname": "voting",
      "expiration": "2018-04-13T10:08:30",
      "proposals": [{
          "pname": "first",
          "description": "the first proposal",
          "proposer": "more",
          "votes": 0
        },{
          "pname": "second",
          "description": "the second proposal",
          "proposer": "more",
          "votes": 0
        }
      ]
    }
  ],
  "more": false
}
```

### Add a proposal

Edit file `propose.json`

```json
{
    "proposer": "moreb",
    "creator": "more",
    "vname": "voting",
    "content": {
        "pname": "selectb",
        "description": "moreb test proposal"
    }
}
```

Run the command

```shell
$ cleos push action more.voting propose "$(cat propose.json)" -p moreb
```

Re-query `vrecord` table for user `more` to view the added proposal

```shell
$ cleos get table more.voting more vrecord
{
  "rows": [{
      "vname": "voting",
      "expiration": "2018-04-13T10:08:30",
      "proposals": [{
          "pname": "first",
          "description": "the first proposal",
          "proposer": "more",
          "votes": 0
        },{
          "pname": "second",
          "description": "the second proposal",
          "proposer": "more",
          "votes": 0
        },{
          "pname": "selectb",
          "description": "moreb test proposal",
          "proposer": "moreb",
          "votes": 0
        }
      ]
    }
  ],
  "more": false
}
```

### Cancel ad voting

Run the command

```shell
$ cleos push action more.voting cancel '{"creator":"morea","vname":"voting"}' -p morea
```

