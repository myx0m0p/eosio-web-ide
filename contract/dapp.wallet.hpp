#pragma once

#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>

namespace DAPP {
    using namespace eosio;
    using std::string;

    class [[eosio::contract("dapp.wallet")]] dapp_wallet : public eosio::contract {
    public:
        dapp_wallet(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds), asset_symbol("UOS", 4) {}

        [[eosio::on_notify("eosio.token::transfer")]]
        inline void deposit(name from, name to, asset quantity, string memo);

        //[[eosio::action]]
        //void send(name account_name, asset quantity);

        [[eosio::action]]
        inline void withdraw(name account_name, asset quantity);

    private:

        const symbol asset_symbol;

        struct [[eosio::table]] balance_entry {
            name account_name;
            eosio::asset balance;

            uint64_t primary_key() const { return account_name.value; }

            EOSLIB_SERIALIZE(balance_entry, (account_name)(balance))
        };

        typedef multi_index <"balance"_n, balance_entry> balance_table;

    };
}