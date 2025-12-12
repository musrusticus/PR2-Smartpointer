#include "bank_account.h"
#include "bank_customer.h" // needed because Account ctor uses Customer::get_id()

#include <stdexcept>
#include <sstream>

// static member definition
unsigned Account::next_id = 0;


std::ostream& operator<<(std::ostream& o, Account_Type at) {
    switch (at) {
        case Account_Type::STANDARD: o << "Standard"; break;
        case Account_Type::SPECIAL:  o << "Special";  break;
    }
    return o;
}

Account::Account(const std::string name_, int dispo_, int amount_, std::shared_ptr<Customer> owner)
    : id(0), name(name_), dispo(dispo_), amount(amount_)
{
    if (name_.empty())
        throw std::runtime_error("Account: name mustnt be empty");
    if (!(dispo_ > 0 && dispo_ < 10000))
        throw std::runtime_error("Account: dispo must be between > 0 and < 10000");
    if (amount_ < -dispo_)
        throw std::runtime_error("Account: amount must be >= -dispo");
    if (!owner)
        throw std::runtime_error("Account: owner mustnt be null");

    // Insert owner (key = owner id)
    unsigned oid = owner->get_id();
    owners[oid] = std::weak_ptr<Customer>(owner);

    id = next_id++;
}

Account::~Account() = default;

int Account::withdraw(int x) {
    if (x <= 0) throw std::runtime_error("Account::withdraw: x must be > 0");
    if (amount - x < -dispo) throw std::runtime_error("Account::withdraw: dispo error");
    amount -= x;
    return amount;
}

int Account::deposit(int x) {
    if (x <= 0) throw std::runtime_error("Account::deposit: x must be > 0");
    amount += x;
    return amount;
}

unsigned Account::get_id() const { return id; }

unsigned Account::owner_count() const {
    unsigned cnt = 0;
    for (const auto& kv : owners) if (!kv.second.expired()) ++cnt;
    return cnt;
}

bool Account::share_account(std::shared_ptr<Customer> new_owner) {
    if (!new_owner) return false;
    unsigned nid = new_owner->get_id();
    auto it = owners.find(nid);
    if (it != owners.end() && !it->second.expired()) return false; // already present and alive
    owners[nid] = std::weak_ptr<Customer>(new_owner);
    return true;
}

bool Account::remove_owner(unsigned owner_id) {
    auto it = owners.find(owner_id);
    if (it == owners.end()) return false;

    // count how many alive owners would remain after removal
    unsigned alive = 0;
    for (const auto& kv : owners) {
        if (kv.first == owner_id) continue;
        if (!kv.second.expired()) ++alive;
    }
    if (alive == 0) return false;

    owners.erase(it);
    return true;
}

const std::string Account::get_name() const { return name; }
int Account::get_amount() const { return amount; }
int Account::get_dispo() const { return dispo; }
const std::map<unsigned, std::weak_ptr<Customer>>& Account::get_owners() const { return owners; }

// prints owners as: [Name, total_amount], [Name2, total_amount2], ... and only non expired owners
static void print_owner_list(std::ostream& o, const std::map<unsigned, std::weak_ptr<Customer>>& owners) {
    bool first = true;
    for (const auto& kv : owners) {
        std::shared_ptr<Customer> owner_sp = kv.second.lock();
        if (!owner_sp) continue;                 // expired -> skip

        if (!first) o << ", ";
        o << "[" << owner_sp->get_name() << ", " << owner_sp->total_amount() << "]";
        first = false;
    }
}

std::ostream& operator<<(std::ostream& o, const Account& p) {
    o << "[" << p.get_name() << ", " << p.additional_output() << ", " << p.get_amount()
      << ", " << p.get_dispo() << ", {";
    print_owner_list(o, p.get_owners());
    o << "}, " << p.owner_count() << "]";
    return o;
}



Standard_Account::Standard_Account(std::string name, int dispo, int amount, std::shared_ptr<Customer> owner)
    : Account(name, dispo, amount, owner)
{}

std::string Standard_Account::additional_output() const {
    return "Standard";
}



Special_Account::Special_Account(std::string name, int dispo, int amount,
                                 std::shared_ptr<Customer> owner, int fee_)
    : Account(name, dispo, amount, owner), fee(fee_)
{
    if (fee_ <= 0) throw std::runtime_error("Special_Account: fee must be > 0");
}

int Special_Account::withdraw(int x) {
    if (x <= 0) throw std::runtime_error("Special_Account::withdraw: x must be > 0");
    int total = x + fee;
    // use base class withdraw which enforces dispo rules
    return Account::withdraw(total);
}

std::string Special_Account::additional_output() const {
    return "Special, " + std::to_string(fee);
}
