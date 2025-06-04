# 📦 Inventory Management System (C++ GUI)

This is a **basic inventory management desktop application** built with **C++**, using a graphical interface powered by **Dear ImGui** and **SDL2**, and data persistence handled through **SQLite**.

This project was completed as part of a university assignment and guided with the help of **ChatGPT** (by OpenAI).

---

## 🚀 Features

- ➕ Add Product
- 📋 View All Products
- 🔍 Search Product by Name
- ✏️ Update Product Info
- ❌ Delete Product
- 💾 SQLite-based persistent database
- 🎨 User-friendly GUI with clean layout and table styling
- ✅ Input validation and success/error messages

---

## 🛠️ Technologies Used

| Component      | Description                          |
|----------------|--------------------------------------|
| C++            | Core application logic               |
| SDL2           | Window, rendering, and input support |
| OpenGL         | Rendering backend for ImGui          |
| Dear ImGui     | Modern, fast, and minimal GUI        |
| SQLite         | Lightweight embedded database        |
| CMake          | Build system for cross-platform dev  |

---


---

## 🖼️ GUI Preview

The app window uses tabs for navigation:
- **Add Product**: Add a new item
- **View Products**: List all entries in a styled table
- **Search**: Lookup by name
- **Update / Delete**: Modify or remove products

Table rows are striped, headers are styled, and icons (emojis) are used to make the interface more friendly.

---

## 🤖 Credits

> 🙌 This project was developed with the **help of ChatGPT**, which provided step-by-step guidance on CMake setup, GUI integration, SQLite usage, and UI improvements.

---

## 📦 Build Instructions (macOS/Linux/Windows)

1. ✅ Install dependencies: SDL2, OpenGL, and CMake
2. ✅ Clone or download the project
3. ✅ Run the following commands:

```bash
cd build
cmake ..
make

./InventoryManager



