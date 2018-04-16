#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;
using namespace std;

typedef uint8_t vcount;

struct proposal_content {

    proposal_content() {}

    name pname;
    string description;

    friend bool operator == ( const proposal_content& a, const proposal_content& b ) {
        return a.pname == b.pname;
    }

    EOSLIB_SERIALIZE( proposal_content, (pname)(description))
};

struct proposal : public proposal_content {

    proposal() {}

    proposal( account_name aname, proposal_content content ) {
        proposer = aname;
        pname = content.pname;
        description = content.description;
    }

    account_name proposer;
    vcount votes = 0;

    EOSLIB_SERIALIZE_DERIVED( proposal, proposal_content, (proposer)(votes))
};

struct proposal_finder {
    name pname;
    proposal_finder(name n) : pname(n) {}
    bool operator() (const proposal& p) { return p.pname == pname; }
};

bool proposal_compare( const proposal& p1, const proposal& p2 ) {
    return p1.votes > p2.votes;
}

class voting : public contract {
    using contract::contract;
public:
    voting( account_name self ) :
            contract( self ){}

    // @abi action
    void create( account_name creator, name vname, time expiration, vector<proposal_content> proposals );

    // @abi action
    void propose( account_name proposer, account_name creator, name vname, proposal_content content );

    // @abi action
    void unpropose( account_name creator, name vname, name pname );

    // @abi action
    void vote( account_name voter, account_name creator, name vname, name pname );

    // @abi reveal
    void reveal( account_name creator, name vname );

    // @abi action
    void cancel( account_name creator, name vname );

private:
    // @abi table
    struct vrecord {
        name vname;
        time expiration;
        vector<proposal> proposals;
        vector<account_name> voters;

        name primary_key() const { return vname; }
    };

    typedef multi_index<N(vrecord), vrecord> vrecords;
};

EOSIO_ABI( voting, (create)(propose)(unpropose)(vote)(reveal)(cancel) )