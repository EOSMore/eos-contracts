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

class voting : public contract {
    using contract::contract;
public:
    voting( account_name self ) :
            contract( self ){}

    // @abi action
    void create( account_name creator, name vname, time expiration, vector<proposal_content> proposals ) {
        require_auth( creator );

        eosio_assert( expiration > now(), "expiration cannot be earlier than current" );

        vrecords record_table(_self, creator);
        eosio_assert( record_table.find( vname ) == record_table.end(), "voting with the same name exists" );

        vector<proposal> i_proposals;

        for (auto prop_itr = proposals.begin(); prop_itr != proposals.end() ; ++prop_itr) {
            proposal i_proposal = proposal(creator, *prop_itr);
            i_proposals.push_back(i_proposal);
        }

        record_table.emplace(creator, [&]( auto& row ) {
            row.vname = vname;
            row.expiration = expiration;
            row.proposals = move(i_proposals);
        });
    }

    // @abi action
    void propose( account_name proposer, account_name creator, name vname, proposal_content content ) {
        require_auth( proposer );

        vrecords record_table(_self, creator);
        auto record_itr = record_table.find( vname );
        eosio_assert( record_itr != record_table.end(), "voting with the name not found" );

        proposal i_proposal = proposal(proposer, content);

        auto prop_itr = std::find( record_itr->proposals.begin(), record_itr->proposals.end(), i_proposal );
        eosio_assert( prop_itr == record_itr->proposals.end(), "proposal with the same name exists" );

        record_table.modify( record_itr, proposer, [&]( auto& row ) {
            row.proposals.push_back( i_proposal );
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
