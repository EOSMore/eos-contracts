#include "more.voting.hpp"

void voting::create(account_name creator, name vname, time expiration, vector<proposal_content> proposals) {
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

void voting::propose( account_name proposer, account_name creator, name vname, proposal_content content ) {
    require_auth( proposer );

    vrecords record_table(_self, creator);
    auto record_itr = record_table.find( vname );
    eosio_assert( record_itr != record_table.end(), "voting with the name not found" );

    eosio_assert( record_itr->expiration < now(), "voting has expired" );

    proposal i_proposal = proposal(proposer, content);

    auto prop_itr = std::find( record_itr->proposals.begin(), record_itr->proposals.end(), i_proposal );
    eosio_assert( prop_itr == record_itr->proposals.end(), "proposal with the same name exists" );

    record_table.modify( record_itr, proposer, [&]( auto& row ) {
        row.proposals.push_back( i_proposal );
    });

}

void voting::unpropose( account_name creator, name vname, name pname ) {

    vrecords record_table(_self, creator);
    auto record_itr = record_table.find( vname );
    eosio_assert( record_itr != record_table.end(), "voting with the name not found" );

    eosio_assert( record_itr->expiration < now(), "voting has expired" );

    auto prop_itr = std::find_if( record_itr->proposals.begin(), record_itr->proposals.end(), proposal_finder(pname) );
    eosio_assert( prop_itr != record_itr->proposals.end(), "proposal with the name not found" );

    account_name proposer = prop_itr->proposer;

    require_auth(proposer);

    record_table.modify( record_itr, proposer, [&]( auto& row ) {
        row.proposals.erase( prop_itr );
    });

}

void voting::vote( account_name voter, account_name creator, name vname, name pname ) {
    require_auth(voter);

    vrecords record_table(_self, creator);
    auto record_itr = record_table.find( vname );
    eosio_assert( record_itr != record_table.end(), "voting with the name not found" );

    eosio_assert( record_itr->expiration > now(), "voting has expired" );

    auto voters_itr = std::find( record_itr->voters.begin(), record_itr->voters.end(), voter );
    eosio_assert( voters_itr == record_itr->voters.end(), "the voter have already voted" );

    auto prop_itr = std::find_if( record_itr->proposals.begin(), record_itr->proposals.end(), proposal_finder(pname) );
    eosio_assert( prop_itr != record_itr->proposals.end(), "proposal with the name not found" );

    record_table.modify( record_itr, voter, [&]( auto& row ) {
        row.voters.push_back( voter );
        auto p_itr = std::find_if( row.proposals.begin(), row.proposals.end(), proposal_finder(pname) );
        p_itr->votes++;
    });

}

void voting::reveal( account_name creator, name vname ) {
    require_auth(creator);

    vrecords record_table(_self, creator);
    auto record_itr = record_table.find( vname );
    eosio_assert( record_itr != record_table.end(), "voting with the name not found" );

    record_table.modify( record_itr, creator, [&]( auto& row ) {
        std::sort( row.proposals.begin(), row.proposals.end(), proposal_compare );
    });

}

void voting::cancel( account_name creator, name vname ) {
    require_auth(creator);

    vrecords record_table(_self, creator);
    auto iterator = record_table.find( vname );
    eosio_assert( iterator != record_table.end(), "voting with the name not found" );

    record_table.erase( iterator );
}
