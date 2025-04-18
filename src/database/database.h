//
// Created by Senad Lemes Galera on 01.04.25.
//
// -------------------------------------------------------------------------------------
#ifndef BUFFER_H
#define BUFFER_H
// -------------------------------------------------------------------------------------
// In this file include all declaration of the objects
// you want to store in the database for column storage
// already implement the add functions inline
// -------------------------------------------------------------------------------------
#include <vector>
// -------------------------------------------------------------------------------------
template <unsigned len>
struct Char
{
    char data[len];
};
// -------------------------------------------------------------------------------------
struct Customer
{
    unsigned id;
    Char<30> name;
    double rabatt;
    unsigned country;
};
// -------------------------------------------------------------------------------------
struct Product
{
    unsigned id;
    Char<30> name;
    double price;
    int amount;
};
// -------------------------------------------------------------------------------------
struct Shop
{
    unsigned id;
    unsigned country;
    unsigned city;
};
// -------------------------------------------------------------------------------------
struct Producer
{
    unsigned id;
    Char<30> name;
    unsigned country;
    double provision;
};
// -------------------------------------------------------------------------------------
struct C_Customers
{
    std::vector<unsigned> data_id;
    std::vector<Char<30>> data_name;
    std::vector<double> data_rabatt;
    std::vector<unsigned> data_country;
    void insert(Customer&& customer);
};
// -------------------------------------------------------------------------------------
struct C_Products
{
    std::vector<unsigned> data_id;
    std::vector<Char<30>> data_name;
    std::vector<double> data_price;
    std::vector<int> data_amount;
    void insert(Product&& product);
};
// -------------------------------------------------------------------------------------
struct C_Shops
{
    std::vector<unsigned> data_id;
    std::vector<unsigned> data_country;
    std::vector<unsigned> data_city;
    void insert(Shop&& shop);
};
// -------------------------------------------------------------------------------------
struct C_Producers
{
    std::vector<unsigned> data_id;
    std::vector<Char<30>> data_name;
    std::vector<unsigned> data_country;
    std::vector<double> data_provision;
    void insert(Producer&& producer);
};
// -------------------------------------------------------------------------------------
// Implementations below here
// -------------------------------------------------------------------------------------
inline void C_Customers::insert(Customer&& customer)
{
    data_id.push_back(customer.id);
    data_name.push_back(customer.name);
    data_rabatt.push_back(customer.rabatt);
    data_country.push_back(customer.country);
}
// -------------------------------------------------------------------------------------
inline void C_Products::insert(Product&& product)
{
    data_id.push_back(product.id);
    data_name.push_back(product.name);
    data_price.push_back(product.price);
    data_amount.push_back(product.amount);
}
// -------------------------------------------------------------------------------------
inline void C_Shops::insert(Shop&& shop)
{
    data_id.push_back(shop.id);
    data_country.push_back(shop.country);
    data_city.push_back(shop.city);
}
// -------------------------------------------------------------------------------------
inline void C_Producers::insert(Producer&& producer)
{
    data_id.push_back(producer.id);
    data_name.push_back(producer.name);
    data_country.push_back(producer.country);
    data_provision.push_back(producer.provision);
}
// -------------------------------------------------------------------------------------
#endif //BUFFER_H
// -------------------------------------------------------------------------------------