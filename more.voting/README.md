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
    "creator": "morea",
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
$ cleos push action more.voting create "$( cat create.json)" -p morea
```
Then query the `vrecord` table to view the added voting

```shell
$ cleos get table more.voting morea vrecord
{
  "rows": [{
      "vname": "voting",
      "expiration": "2018-04-13T10:08:30",
      "proposals": [{
          "pname": "first",
          "description": "the first proposal",
          "proposer": "morea",
          "votes": 0
        },{
          "pname": "second",
          "description": "the second proposal",
          "proposer": "morea",
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
    "creator": "morea",
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

Re-query `vrecord` table for user `morea` to view the added proposal

```shell
$ cleos get table more.voting morea vrecord
{
  "rows": [{
      "vname": "voting",
      "expiration": "2018-04-13T10:08:30",
      "proposals": [{
          "pname": "first",
          "description": "the first proposal",
          "proposer": "morea",
          "votes": 0
        },{
          "pname": "second",
          "description": "the second proposal",
          "proposer": "morea",
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

