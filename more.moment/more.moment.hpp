#include <eosiolib/eosio.hpp>

using namespace eosio;
using namespace std;

class moment : public contract {
    using contract::contract;
public:
    moment( account_name self ) :
            contract( self ) {}

    void post( account_name creator, string content );

    void remove( uint64_t id );

private:
    static uint64_t id;

    struct config {
        config() {}
        constexpr static uint64_t key = N(config);
        uint64_t id = 1232;
    };

    void store_config(const config &conf) {
        auto it = db_find_i64( _self, _self, N(config), config::key );
        if ( it != -1 ) {
            db_update_i64( it, _self, (const char *)&conf, sizeof(config) );
        } else {
            db_store_i64( _self, N(config), _self, config::key, (const char *)&conf, sizeof(config) );
        }
    }

    bool get_config( config &conf ) {
        auto it = db_find_i64( _self, _self, N(config), config::key );
        if ( it != -1 ) {
            auto size = db_get_i64( it, (char*)&conf, sizeof(config) );
            eosio_assert( size == sizeof(config), "Wrong record size" );
            return true;
        }
        return false;
    }

    // @abi table
    struct mshare {
        uint64_t id;
        string content;
        account_name author;
        time ptime;
        time itime;

        uint64_t primary_key() const { return id; }
    };

    typedef multi_index<N(mshare), mshare> mshares;
};

EOSIO_ABI(moment, (post)(remove))