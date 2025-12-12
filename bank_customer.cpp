// bank_customer.cpp
#include "bank_customer.h"
#include "bank_account.h"

#include <stdexcept>
#include <sstream>

unsigned Customer::next_id = 0;


Customer::Customer(std::string name_) : id(0), name(name_) {
    if (name_.empty()) throw std::runtime_error("Customer: name mustnt be empty");
    // only assign id after validation
    id = next_id++;
}


unsigned Customer::create_account(std::string acc_name, int dispo, int amount, Account_Type type, int fee)
{
    // need shared_ptr to this for account owner parameter
    std::shared_ptr<Customer> self;
    try {
        self = this->shared_from_this();
    } catch (const std::bad_weak_ptr&) {
        throw std::runtime_error("create_account: Customer not owned by shared_ptr");
    }

    std::shared_ptr<Account> acc;
    if (type == Account_Type::STANDARD) {
        acc = std::make_shared<Standard_Account>(acc_name, dispo, amount, self);
    } else { // SPECIAL
        acc = std::make_shared<Special_Account>(acc_name, dispo, amount, self, fee);
    }

    unsigned aid = acc->get_id();
    accounts[aid] = acc;
    return aid;
}

int Customer::total_amount() const {
    long long sum = 0;
    for (const auto& kv : accounts) {
        if (kv.second) sum += kv.second->get_amount();
    }
    return static_cast<int>(sum);
}

unsigned Customer::get_id() const {
    return id;
}

const std::string& Customer::get_name() const {
    return name;
}

const std::map<unsigned, std::shared_ptr<Account>>& Customer::get_accounts() const {
    return accounts;
}

bool Customer::share_account(unsigned aid, std::shared_ptr<Customer> new_owner) {
    auto it = accounts.find(aid);
    if (it == accounts.end()) return false;
    auto acc = it->second;
    if (!acc || !new_owner) return false;

    bool ok = acc->share_account(new_owner);
    if (!ok) return false;

    // add account into new_owner's accounts map
    new_owner->accounts[aid] = acc;
    return true;
}


bool Customer::transfer(int amount, unsigned source_id, std::shared_ptr<Customer> target, unsigned target_id) {
    if (source_id == target_id) throw std::runtime_error("transfer: source_id and target_id are equal");
    if (!target) return false;

    auto it_src = accounts.find(source_id);
    if (it_src == accounts.end()) return false;
    auto src_acc = it_src->second;
    if (!src_acc) return false;

    auto it_tgt = target->accounts.find(target_id);
    if (it_tgt == target->accounts.end()) return false;
    auto tgt_acc = it_tgt->second;
    if (!tgt_acc) return false;

    // withdraw then deposit; attempt rollback on deposit failure
    try {
        src_acc->withdraw(amount);
    } catch (...) {
        return false;
    }

    try {
        tgt_acc->deposit(amount);
    } catch (...) {
        // rollback: attempt to restore src_acc (best-effort)
        try {
            src_acc->deposit(amount);
        } catch (...) {
            // leave inconsistent if rollback fails (very unlikely)
        }
        return false;
    }

    return true;
}


std::ostream& operator<<(std::ostream& o, const Customer& c) {
    o << "[" << c.name << ", {";
    bool first = true;
    for (const auto& kv : c.accounts) {
        const auto& acc = kv.second;
        if (!acc) continue;
        if (!first) o << ", ";
        o << "[" << acc->get_name() << ", " << acc->owner_count() << "]";
        first = false;
    }
    o << "}, " << c.total_amount() << "]";
    return o;
}
