#include "dapp.wallet.hpp"

namespace DAPP {

    using namespace eosio;
    using std::string;

    void dapp_wallet::deposit(name from, name to, asset quantity, string memo) {
        print("TRANSFER\n");
        print("FROM ", name{from}, "\n");
        print("TO ", name{to}, "\n");
        print("QUANTITY ", quantity.to_string(), "\n");
        print("AMOUNT ", quantity.amount, "\n");
        print("SYMBOL CODE ", quantity.symbol.code().to_string(), "\n");
        print("MEMO ", memo, "\n");

        if(from == _self) {
            //print("IGNORE OUTGOING TRANSFER\n");
            return;
        }

        check(quantity.is_valid(), "invalid quantity" );
        check(quantity.amount > 0, "Amount should be greater then zero");
        check(quantity.symbol == dapp_wallet::asset_symbol, "Only UOS core tokens accepted");

        auto account_for = from;

        if(is_account(name{memo})) {
            account_for = eosio::name(memo);
        }
        //check(is_account(name{memo}), "must be an existing account name in memo");

        balance_table balances(get_self(), get_self().value);


        auto iterator = balances.find(account_for.value);
        
        if(iterator != balances.end()) {
            print("FOUND!!!!!\n");
            balances.modify(iterator, get_self(), [&](auto& item){
                item.account_name = account_for;
                item.balance += quantity;
            });
            print("AND MODIFIED\n");
        } else {
            print("NOT FOUND!!!!!\n");
            balances.emplace(get_self(), [&](auto& item){
                item.account_name = account_for;
                item.balance = quantity;
            });
            print("AND ADDED\n");
        }
    }

    void dapp_wallet::withdraw(name account_name, asset quantity) {
        print("WITHDRAW","\n");
        print("ACC_NAME ", name{account_name}, "\n");
        print("QUANTITY ", quantity.to_string(), "\n");
        print("AMOUNT ", quantity.amount, "\n");
        print("SYMBOL CODE ", quantity.symbol.code().to_string(), "\n");

        require_auth(account_name);

        check(quantity.is_valid(), "invalid quantity" );
        check(quantity.amount > 0, "Amount should be greater then zero");
        check(quantity.symbol == dapp_wallet::asset_symbol, "Only UOS core tokens accepted");       
        
        
        balance_table balances(get_self(), get_self().value);

        auto iterator = balances.find(account_name.value);
        
        check(iterator != balances.end(), "balance record not found");

        check(iterator->balance.amount >= quantity.amount, "amount exceed");

        action(
            permission_level{ get_self(), name{"active"} },
            name{"eosio.token"}, 
            name{"transfer"},
            std::make_tuple(get_self(), account_name, quantity, string("withdraw from wallet"))
        ).send();
        
        if(iterator->balance.amount - quantity.amount == 0) {
            //erase balance
            balances.erase(iterator);
        } else {
            //decreace balance
            balances.modify(iterator, get_self(), [&](auto& item){
                item.balance -= quantity;            
            });  
        }
      
    }
    
    /*
    extern "C" {
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        dapp_wallet _dapp_wallet((name(receiver)));
        if(code==receiver && action== name("send").value) {
            execute_action(name(receiver), name(code), &dapp_wallet::send );
        }
        else if(code==receiver && action== name("withdraw").value) {
            execute_action(name(receiver), name(code), &dapp_wallet::withdraw );
        }
        else if(code==name("eosio.token").value && action== name("transfer").value) {
        execute_action(name(receiver), name(code), &dapp_wallet::transfer );
        }
    }
    }
    */
}