#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;
using namespace std;

typedef uint8_t vcount;

struct proposal {
    account_name proposer;
    name pname;
    string description;
    vcount votes = 0;

    friend bool operator == ( const proposal& a, const proposal& b ) {
        return a.pname == b.pname;
    }

    EOSLIB_SERIALIZE( proposal, (proposer)(pname)(description)(votes))
};

class voting : public contract {
    using contract::contract;
public:
    voting( account_name self ) :
            contract( self ){}

    // @abi action
    void create( account_name creator, name vname, time expiration, vector<proposal> proposals = {} ) {
        require_auth( creator );

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
    void propose( account_name proposer, account_name creator, name vname, proposal proposal ) {
        require_auth( proposer );

        vrecords record_table(_self, creator);
        auto record_itr = record_table.find( vname );
        eosio_assert( record_itr != record_table.end(), "voting not found" );

        auto prop_itr = std::find( record_itr->proposals.begin(), record_itr->proposals.end(), proposal );
        eosio_assert( prop_itr == record_itr->proposals.end(), "proposal with the same name exists" );

        record_table.modify( record_itr, proposer, [&]( auto& row ) {
            row.proposals.push_back( proposal );
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

EOSIO_ABI( voting, (create)(propose)(cancel) )
