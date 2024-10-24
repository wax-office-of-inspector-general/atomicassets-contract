#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <checkformat.hpp>
#include <atomicdata.hpp>

using namespace eosio;
using namespace std;
using namespace atomicdata;


static constexpr double MAX_MARKET_FEE = 0.15;
static constexpr uint32_t AUTHOR_SWAP_TIME_DELTA = 60 * 60 * 24 * 7; // 1 week, valid for 1 week

static const string MISSING_COLLECTION_AUTH = "Missing authorization for this collection";
static constexpr char COLLECTION_NOT_FOUND[] = "No collection with this name exists";

CONTRACT atomicassets : public contract {
public:
    using contract::contract;

    ACTION init();

    ACTION admincoledit(vector <FORMAT> collection_format_extension);

    ACTION setversion(string new_version);

    ACTION addconftoken(name token_contract, symbol token_symbol);

    ACTION transfer(
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );

    ACTION move(
        name owner,
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );

    ACTION createcol(
        name author,
        name collection_name,
        bool allow_notify,
        vector <name> authorized_accounts,
        vector <name> notify_accounts,
        double market_fee,
        ATTRIBUTE_MAP data
    );

    ACTION setcoldata(
        name collection_name,
        ATTRIBUTE_MAP data
    );

    ACTION addcolauth(
        name collection_name,
        name account_to_add
    );

    ACTION remcolauth(
        name collection_name,
        name account_to_remove
    );

    ACTION addnotifyacc(
        name collection_name,
        name account_to_add
    );

    ACTION remnotifyacc(
        name collection_name,
        name account_to_remove
    );

    ACTION setmarketfee(
        name collection_name,
        double market_fee
    );

    ACTION forbidnotify(
        name collection_name
    );

    ACTION createauswap(
        name collection_name,
        name new_author,
        bool owner
    );

    ACTION acceptauswap(
        name collection_name
    );

    ACTION rejectauswap(
        name collection_name
    );

    ACTION createschema(
        name authorized_creator,
        name collection_name,
        name schema_name,
        vector <FORMAT> schema_format
    );

    ACTION extendschema(
        name authorized_editor,
        name collection_name,
        name schema_name,
        vector <FORMAT> schema_format_extension
    );

    ACTION setschematyp(
        name authorized_editor,
        name collection_name,
        name schema_name,
        vector <FORMAT_TYPE> schema_format_type
    );

    ACTION createtempl(
        name authorized_creator,
        name collection_name,
        name schema_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP immutable_data
    );
    
    ACTION createtempl2(
        name authorized_creator,
        name collection_name,
        name schema_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data
    );

    ACTION settempldata(
        name authorized_editor,
        name collection_name,
        int32_t template_id,
        ATTRIBUTE_MAP new_mutable_data
    );

    ACTION deltemplate(
        name authorized_editor,
        name collection_name,
        int32_t template_id
    );

    ACTION locktemplate(
        name authorized_editor,
        name collection_name,
        int32_t template_id
    );

    ACTION redtemplmax(
        name authorized_editor,
        name collection_name,
        int32_t template_id,
        uint32_t new_max_supply
    );

    ACTION mintasset(
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data,
        vector <asset> tokens_to_back
    );

    ACTION setassetdata(
        name authorized_editor,
        name asset_owner,
        uint64_t asset_id,
        ATTRIBUTE_MAP new_mutable_data
    );


    ACTION announcedepo(
        name owner,
        symbol symbol_to_announce
    );

    ACTION withdraw(
        name owner,
        asset token_to_withdraw
    );

    ACTION backasset(
        name payer,
        name asset_owner,
        uint64_t asset_id,
        asset token_to_back
    );

    ACTION burnasset(
        name asset_owner,
        uint64_t asset_id
    );


    ACTION createoffer(
        name sender,
        name recipient,
        vector <uint64_t> sender_asset_ids,
        vector <uint64_t> recipient_asset_ids,
        string memo
    );

    ACTION canceloffer(
        uint64_t offer_id
    );

    ACTION acceptoffer(
        uint64_t offer_id
    );

    ACTION declineoffer(
        uint64_t offer_id
    );

    ACTION payofferram(
        name payer,
        uint64_t offer_id
    );

    [[eosio::on_notify("*::transfer")]] void receive_token_transfer(
        name from,
        name to,
        asset quantity,
        string memo
    );

    ACTION logtransfer(
        name collection_name,
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );

    ACTION lognewoffer(
        uint64_t offer_id,
        name sender,
        name recipient,
        vector <uint64_t> sender_asset_ids,
        vector <uint64_t> recipient_asset_ids,
        string memo
    );

    ACTION lognewtempl(
        int32_t template_id,
        name authorized_creator,
        name collection_name,
        name schema_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP immutable_data
    );

    ACTION logmint(
        uint64_t asset_id,
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data,
        vector <asset> backed_tokens,
        ATTRIBUTE_MAP immutable_template_data
    );

    ACTION logsetdata(
        name asset_owner,
        uint64_t asset_id,
        ATTRIBUTE_MAP old_data,
        ATTRIBUTE_MAP new_data
    );

    ACTION logsetdatatl(
        name collection_name,
        name schema_name,
        int32_t template_id, 
        ATTRIBUTE_MAP old_data,
        ATTRIBUTE_MAP new_data
    );

    ACTION logbackasset(
        name asset_owner,
        uint64_t asset_id,
        asset backed_token
    );

    ACTION logburnasset(
        name asset_owner,
        uint64_t asset_id,
        name collection_name,
        name schema_name,
        int32_t template_id,
        vector <asset> backed_tokens,
        ATTRIBUTE_MAP old_immutable_data,
        ATTRIBUTE_MAP old_mutable_data,
        name asset_ram_payer
    );


private:

    TABLE author_swaps_s {
        name             collection_name;
        name             current_author;
        name             new_author;
        uint32_t         acceptance_date;

        uint64_t primary_key() const { return collection_name.value; };
    };

    typedef multi_index <name("authorswaps"), author_swaps_s> author_swaps_t;

    TABLE collections_s {
        name             collection_name;
        name             author;
        bool             allow_notify;
        vector <name>    authorized_accounts;
        vector <name>    notify_accounts;
        double           market_fee;
        vector <uint8_t> serialized_data;

        uint64_t primary_key() const { return collection_name.value; };
    };

    typedef multi_index <name("collections"), collections_s> collections_t;


    //Scope: collection_name
    TABLE schemas_s {
        name            schema_name;
        vector <FORMAT> format;

        uint64_t primary_key() const { return schema_name.value; }
    };

    typedef multi_index <name("schemas"), schemas_s> schemas_t;

    //Scope: collection_name
    TABLE schema_types_s {
        name            schema_name;
        vector <FORMAT_TYPE> format_type;

        uint64_t primary_key() const { return schema_name.value; }
    };

    typedef multi_index <name("schematypes"), schema_types_s> schema_types_t;

    //Scope: collection_name
    TABLE templates_s {
        int32_t          template_id;
        name             schema_name;
        bool             transferable;
        bool             burnable;
        uint32_t         max_supply;
        uint32_t         issued_supply;
        vector <uint8_t> immutable_serialized_data;

        uint64_t primary_key() const { return (uint64_t) template_id; }
    };

    typedef multi_index <name("templates"), templates_s> templates_t;

    //Scope: collection_name
    TABLE template_mutables_s {
        int32_t          template_id;
        name             schema_name;
        vector <uint8_t> mutable_serialized_data;

        uint64_t primary_key() const { return (uint64_t) template_id; }
    };

    typedef multi_index <name("tmplmutables"), template_mutables_s> template_mutables_t;

    //Scope: owner
    TABLE assets_s {
        uint64_t         asset_id;
        name             collection_name;
        name             schema_name;
        int32_t          template_id;
        name             ram_payer;
        vector <asset>   backed_tokens;
        vector <uint8_t> immutable_serialized_data;
        vector <uint8_t> mutable_serialized_data;

        uint64_t primary_key() const { return asset_id; };
    };

    typedef multi_index <name("assets"), assets_s> assets_t;

    TABLE holders_s {
        uint64_t         asset_id;
        name             holder;
        name             owner;

        uint64_t primary_key() const { return asset_id; };
        uint64_t by_holder() const { return holder.value; };
    };

    typedef multi_index <name("holders"), holders_s,  
        indexed_by<name("holder"), const_mem_fun <holders_s, uint64_t, &holders_s::by_holder>>>
    holders_t;

    TABLE offers_s {
        uint64_t          offer_id;
        name              sender;
        name              recipient;
        vector <uint64_t> sender_asset_ids;
        vector <uint64_t> recipient_asset_ids;
        string            memo;
        name              ram_payer;

        uint64_t primary_key() const { return offer_id; };

        uint64_t by_sender() const { return sender.value; };

        uint64_t by_recipient() const { return recipient.value; };
    };

    typedef multi_index <name("offers"), offers_s,
        indexed_by < name("sender"), const_mem_fun < offers_s, uint64_t, &offers_s::by_sender>>,
    indexed_by <name("recipient"), const_mem_fun < offers_s, uint64_t, &offers_s::by_recipient>>>
    offers_t;

    TABLE balances_s {
        name           owner;
        vector <asset> quantities;

        uint64_t primary_key() const { return owner.value; };
    };

    typedef multi_index <name("balances"), balances_s>         balances_t;


    TABLE config_s {
        uint64_t                 asset_counter     = 1099511627776; //2^40
        int32_t                  template_counter  = 1;
        uint64_t                 offer_counter     = 1;
        vector <FORMAT>          collection_format = {};
        vector <extended_symbol> supported_tokens  = {};
    };
    typedef singleton <name("config"), config_s>               config_t;


    TABLE tokenconfigs_s {
        name        standard = name("atomicassets");
        std::string version  = string("1.3.1");
    };
    typedef singleton <name("tokenconfigs"), tokenconfigs_s>   tokenconfigs_t;

    // Table Fetches
    author_swaps_t      get_author_swaps() {return author_swaps_t(get_self(), get_self().value);}
    collections_t       get_collections() {return collections_t(get_self(), get_self().value);}
    collections_t       get_collections_data() {return collections_t(get_self(), name("coldata").value);}

    offers_t            get_offers() {return offers_t(get_self(), get_self().value);}
    balances_t          get_balances() {return balances_t(get_self(), get_self().value);}
    config_t            get_config() {return config_t(get_self(), get_self().value);}
    tokenconfigs_t      get_tokenconfigs() {return tokenconfigs_t(get_self(), get_self().value);}

    schemas_t           get_schemas(name collection_name) {return schemas_t(get_self(), collection_name.value);}
    schema_types_t      get_schema_types(name collection_name) {return schema_types_t(get_self(), collection_name.value);}

    templates_t         get_templates(name collection_name) {return templates_t(get_self(), collection_name.value);}
    template_mutables_t get_template_mutables(name collection_name) {return template_mutables_t(get_self(), collection_name.value);}

    assets_t            get_assets(name owner) {return assets_t(get_self(), owner.value);}
    holders_t           get_holders() {return holders_t(get_self(), get_self().value);}

    // Internal Functions

    void internal_create_template(
        name authorized_creator,
        name collection_name,
        name schema_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP & immutable_data,
        ATTRIBUTE_MAP mutable_data = {}
    );

    void internal_transfer(
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo,
        name scope_payer
    );

    void internal_decrease_balance(
        name owner,
        asset quantity
    );

    void notify_collection_accounts(
        name collection_name
    );

    void check_has_collection_auth(
        name & account_to_check,
        const collections_s & collection_itr
    );

    void check_name_length(ATTRIBUTE_MAP & data);

    void coldata_cleanup(name & collection_name, const collections_s & collection_itr);

};
