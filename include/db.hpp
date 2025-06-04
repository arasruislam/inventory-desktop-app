#ifndef DB_HPP
#define DB_HPP

#include <string>
#include <vector>

// Product structure
struct Product {
    int id;
    std::string name;
    int quantity;
    double price;
};

// Database function declarations
bool initDB(const std::string& dbName);
bool addProduct(const Product& product);
// bool deleteProduct(int productId);
// bool updateProduct(const Product& product);
std::vector<Product> getAllProducts();
// std::vector<Product> searchProducts(const std::string& keyword);

// Search products by name (contains) or ID
std::vector<Product> searchProducts(const std::string& keyword);

// Get a product by ID
Product getProductById(int id);

// Update product
bool updateProduct(const Product& p);

// Delete product
bool deleteProduct(int id);

#endif // DB_HPP
