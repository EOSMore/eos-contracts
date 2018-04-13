#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;
using namespace std;

typedef uint8_t vcount;

struct proposal {
    name pname;
    string description;
    vcount votes = 0;
    EOSLIB_SERIALIZE( proposal, (pname)(description)(votes))
};

class voting : public contract {
    using contract::contract;
public:
    voting( account_name self ) :
            contract( self ){}

    // @abi action
    void create( account_name creator, name vname, time expiration, vector<proposal> proposals = {} ) {
        require_auth(creator);

        eosio_assert( expiration > now(), "expiration cannot be earlier than current" );

        vrecords record_table(_self, creator);
        eosio_assert( record_table.find( vname ) == record_table.end(), "voting with the same name exists" );

        record_table.emplace(creator, [&]( auto& row ) {
            row.vname = vname;
            row.expiration = expiration;
            row.proposals = move(proposals);
        });
    }

    // @abi action
    void cancel( account_name creator, name vname ) {
        require_auth(creator);

        vrecords record_table(_self, creator);
        auto iterator = record_table.find( vname );
        eosio_assert( iterator != record_table.end(), "voting not found" );

        record_table.erase( iterator );
    }

private:
    // @abi table
    struct vrecord {
        name vname;
        time expiration;
        vector<proposal> proposals;

        name primary_key() const { return vname; }
    };

    typedef multi_index<N(vrecord), vrecord> vrecords;
};

EOSIO_ABI( voting, (create)(cancel) )
