#include "more.moment.hpp"

uint64_t moment::id = 2;

void moment::post(account_name creator, string content) {
    require_auth(creator);

    mshares mshare_table(_self, _self);

    config conf;
    get_config(conf);

    uint64_t id = conf.id++;

    mshare_table.emplace( creator, [&]( auto& mshare ) {
        mshare.id = id;
        mshare.content = content;
        mshare.author = creator;
        mshare.ptime = now();
        mshare.itime = now();
    });

    store_config(conf);

}