#include "db.hpp"
#include <sqlite3.h>
#include <iostream>

static sqlite3* db;

bool initDB(const std::string& dbName) {
    int result = sqlite3_open(dbName.c_str(), &db);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to open DB: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            quantity INTEGER NOT NULL,
            price REAL NOT NULL
        );
    )";

    char* errMsg;
    result = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        std::cerr << "Failed to create table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool addProduct(const Product& product) {
    const char* sql = "INSERT INTO products (name, quantity, price) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, product.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, product.quantity);
    sqlite3_bind_double(stmt, 3, product.price);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

// bool deleteProduct(int productId) {
//     const char* sql = "DELETE FROM products WHERE id = ?;";
//     sqlite3_stmt* stmt;

//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
//         return false;

//     sqlite3_bind_int(stmt, 1, productId);

//     bool success = (sqlite3_step(stmt) == SQLITE_DONE);
//     sqlite3_finalize(stmt);
//     return success;
// }

// bool updateProduct(const Product& product) {
//     const char* sql = "UPDATE products SET name = ?, quantity = ?, price = ? WHERE id = ?;";
//     sqlite3_stmt* stmt;

//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
//         return false;

//     sqlite3_bind_text(stmt, 1, product.name.c_str(), -1, SQLITE_STATIC);
//     sqlite3_bind_int(stmt, 2, product.quantity);
//     sqlite3_bind_double(stmt, 3, product.price);
//     sqlite3_bind_int(stmt, 4, product.id);

//     bool success = (sqlite3_step(stmt) == SQLITE_DONE);
//     sqlite3_finalize(stmt);
//     return success;
// }

std::vector<Product> getAllProducts() {
    const char* sql = "SELECT * FROM products;";
    sqlite3_stmt* stmt;
    std::vector<Product> products;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return products;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p;
        p.id = sqlite3_column_int(stmt, 0);
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.quantity = sqlite3_column_int(stmt, 2);
        p.price = sqlite3_column_double(stmt, 3);
        products.push_back(p);
    }

    sqlite3_finalize(stmt);
    return products;
}

// std::vector<Product> searchProducts(const std::string& keyword) {
//     const char* sql = "SELECT * FROM products WHERE name LIKE ?;";
//     sqlite3_stmt* stmt;
//     std::vector<Product> results;

//     if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
//         return results;

//     std::string likeKeyword = "%" + keyword + "%";
//     sqlite3_bind_text(stmt, 1, likeKeyword.c_str(), -1, SQLITE_STATIC);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         Product p;
//         p.id = sqlite3_column_int(stmt, 0);
//         p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
//         p.quantity = sqlite3_column_int(stmt, 2);
//         p.price = sqlite3_column_double(stmt, 3);
//         results.push_back(p);
//     }

//     sqlite3_finalize(stmt);
//     return results;
// }


// date: 03.06.2025
std::vector<Product> searchProducts(const std::string &keyword)
{
    std::vector<Product> results;

    if (!db)
        return results;

    std::string sql;
    bool isNumber = std::all_of(keyword.begin(), keyword.end(), ::isdigit);

    if (isNumber)
    {
        sql = "SELECT * FROM products WHERE id = " + keyword + " OR name LIKE '%" + keyword + "%';";
    }
    else
    {
        sql = "SELECT * FROM products WHERE name LIKE '%" + keyword + "%';";
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Product p;
            p.id = sqlite3_column_int(stmt, 0);
            p.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            p.quantity = sqlite3_column_int(stmt, 2);
            p.price = sqlite3_column_double(stmt, 3);
            results.push_back(p);
        }
        sqlite3_finalize(stmt);
    }

    return results;
}

Product getProductById(int id)
{
    Product p = {-1, "", 0, 0.0};
    if (!db)
        return p;

    std::string sql = "SELECT * FROM products WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            p.id = sqlite3_column_int(stmt, 0);
            p.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            p.quantity = sqlite3_column_int(stmt, 2);
            p.price = sqlite3_column_double(stmt, 3);
        }

        sqlite3_finalize(stmt);
    }

    return p;
}

bool updateProduct(const Product &p)
{
    if (!db)
        return false;

    std::string sql = "UPDATE products SET name = ?, quantity = ?, price = ? WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, p.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, p.quantity);
        sqlite3_bind_double(stmt, 3, p.price);
        sqlite3_bind_int(stmt, 4, p.id);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            return true;
        }

        sqlite3_finalize(stmt);
    }

    return false;
}

bool deleteProduct(int id)
{
    if (!db)
        return false;

    std::string sql = "DELETE FROM products WHERE id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            return true;
        }

        sqlite3_finalize(stmt);
    }

    return false;
}
