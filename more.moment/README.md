# moment contract
## build and deploy
```shell
$ eosiocpp -o contracts/more.moment/more.moment.wast contracts/more.moment/more.moment.cpp
$ cleos set contract more.moment contracts/more.moment -p more.moment
```
## run
### Post
```shell
$ cleos push action more.moment post '{"creator":"more","content":"it's a nice day"}' -p more
```
### View the table
```shell
$ cleos get table more.moment more.moment mshare
{
  "rows": [{
      "id": 1232,
      "content": "it's a nice day",
      "author": "more",
      "ptime": "2018-04-16T09:15:29",
      "itime": "2018-04-16T09:15:29"
    },{
      "id": 1233,
      "content": "生命在于运动",
      "author": "morea",
      "ptime": "2018-04-16T09:15:46",
      "itime": "2018-04-16T09:15:46"
    }
  ],
  "more": false
}
```
### Remove
```shell
$ cleos push action more.moment remove '[1232]' -p more
```