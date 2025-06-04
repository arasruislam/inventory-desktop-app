#include "gui.hpp"
#include "db.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
// #include <SDL.h>
// #include <SDL_opengl.h>
#include <SDL2/SDL.h>
#include <OpenGL/gl3.h>
#include <iostream>

void runGUI()
{
    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Request OpenGL 3.2 Core Profile (important for macOS)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create window
    SDL_Window *window = SDL_CreateWindow("Inventory Manager",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    // Print OpenGL version
    const GLubyte *version = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // UI Styling
    ImGui::StyleColorsDark();
    io.FontGlobalScale = 1.2f;

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.FramePadding = ImVec2(12, 8);
    style.ItemSpacing = ImVec2(12, 10);
    style.WindowPadding = ImVec2(15, 15);

    // Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 150");

    // App state variables
    bool running = true;
    char name[128] = "";
    int quantity = 0;
    float price = 0.0f;

    // For UI feedback messages
    std::string statusMessage = "";
    ImVec4 statusColor = ImVec4(1, 1, 1, 1);

    // Dark mode toggle state
    bool use_dark = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Get SDL window size dynamically for ImGui window sizing
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        ImGui::SetNextWindowSize(ImVec2((float)w, (float)h), ImGuiCond_Always);

        // Begin main window
        ImGui::Begin("📦 Inventory Manager", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

        // Dark mode toggle
        if (ImGui::Checkbox("Dark Mode", &use_dark))
        {
            if (use_dark)
                ImGui::StyleColorsDark();
            else
                ImGui::StyleColorsLight();
        }
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), "Welcome to Your Inventory System");
        ImGui::Spacing();

        // Show status message inside UI
        if (!statusMessage.empty())
        {
            ImGui::TextColored(statusColor, "%s", statusMessage.c_str());
            ImGui::Spacing();
        }

        // Tabs
        if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyScroll))
        {
            if (ImGui::BeginTabItem("➕ Add Product"))
            {
                ImGui::InputText("Product Name", name, IM_ARRAYSIZE(name));
                ImGui::InputInt("Quantity", &quantity);
                ImGui::InputFloat("Price", &price);

                if (ImGui::Button("Add Product", ImVec2(150, 40)))
                {
                    Product p = {0, name, quantity, (double)price};
                    if (addProduct(p))
                    {
                        statusMessage = "✅ Product added!";
                        statusColor = ImVec4(0, 1, 0, 1);
                        // Reset inputs
                        name[0] = '\0';
                        quantity = 0;
                        price = 0.0f;
                    }
                    else
                    {
                        statusMessage = "❌ Failed to add product.";
                        statusColor = ImVec4(1, 0, 0, 1);
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("📋 View Products"))
            {
                // Example product list display using columns
                ImGui::Text("Product List:");
                ImGui::Separator();

                ImGui::Columns(4, "product_columns", true);
                ImGui::Text("ID");
                ImGui::NextColumn();
                ImGui::Text("Name");
                ImGui::NextColumn();
                ImGui::Text("Quantity");
                ImGui::NextColumn();
                ImGui::Text("Price");
                ImGui::NextColumn();

                // Assuming you have a function to get all products:
                auto products = getAllProducts(); // Implement this yourself

                for (const auto &prod : products)
                {
                    ImGui::Text("%d", prod.id);
                    ImGui::NextColumn();
                    ImGui::Text("%s", prod.name.c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%d", prod.quantity);
                    ImGui::NextColumn();
                    ImGui::Text("%.2f", prod.price);
                    ImGui::NextColumn();
                }

                ImGui::Columns(1);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("🔍 Search"))
            {
                renderSearchProduct();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("✏️ Update Product"))
            {
                renderUpdateProduct();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("❌ Delete Product"))
            {
                renderDeleteProduct();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        // Render
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void renderProductList()
{
    static std::vector<Product> products = getAllProducts();

    ImGui::BeginGroup();

    // Title
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "📦 Product List");
    ImGui::Separator();
    ImGui::Spacing();

    // Label + Refresh Button (blue style)
    ImGui::Text("Refresh product list:");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.8f, 1.0f));

    if (ImGui::Button("🔄 Refresh List", ImVec2(160, 35)))
    {
        // Refresh the products by fetching fresh data
        products = getAllProducts();
    }

    ImGui::PopStyleColor(3);
    ImGui::Spacing();

    // Scrollable child for table
    float tableHeight = 400.0f;
    ImGui::BeginChild("ProductTableRegion", ImVec2(0, tableHeight), true);

    if (ImGui::BeginTable("ProductTable", 4,
                          ImGuiTableFlags_Borders |
                              ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_Resizable |
                              ImGuiTableFlags_SizingStretchProp |
                              ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupColumn("🆔 ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("📦 Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("📊 Quantity", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("💵 Price", ImGuiTableColumnFlags_WidthFixed, 80.0f);

        ImGui::TableHeadersRow();

        for (const auto &p : products)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", p.id);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextWrapped("%s", p.name.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%d", p.quantity);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.2f", p.price);
        }

        ImGui::EndTable();
    }

    ImGui::EndChild();

    ImGui::EndGroup();
}

void renderSearchProduct()
{
    static char keyword[128] = "";

    ImGui::Text("🔍 Search for a Product");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Enter Product ID or Name:");
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##SearchBar", keyword, IM_ARRAYSIZE(keyword));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // 🔵 Styled "Clear Search" button - soft blue-gray like Update UI
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));        // Base color
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f)); // Hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));  // Active

    if (ImGui::Button("🔄 Clear Search", ImVec2(150, 35)))
    {
        keyword[0] = '\0';
    }

    ImGui::PopStyleColor(3);
    ImGui::Spacing();

    if (strlen(keyword) > 0)
    {
        std::vector<Product> results = searchProducts(keyword);

        if (results.empty())
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "No products found matching your search.");
        }
        else if (ImGui::BeginTable("SearchTable", 4,
                                   ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_Resizable |
                                       ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("🆔 ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("📦 Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("📊 Quantity", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("💵 Price", ImGuiTableColumnFlags_WidthFixed, 80.0f);

            ImGui::TableHeadersRow();

            for (const auto &p : results)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", p.id);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", p.name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", p.quantity);

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.2f", p.price);
            }

            ImGui::EndTable();
        }
    }
}

void renderUpdateProduct()
{
    static char inputSearch[128] = "";
    static Product loadedProduct = {0, "", 0, 0.0};
    static bool productLoaded = false;
    static bool updateSuccess = false;
    static bool updateFailed = false;

    static char updatedName[128] = "";
    static int updatedQuantity = 0;
    static float updatedPrice = 0.0f;

    ImGui::BeginGroup();

    ImGui::Text("🛠️ Update Product");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Enter Product ID or Name:");
    ImGui::PushItemWidth(-1); // full width input
    ImGui::InputText("##SearchProduct", inputSearch, IM_ARRAYSIZE(inputSearch));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // Styled blue Load button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.4f, 0.8f, 1.0f));

    if (ImGui::Button("🔍 Load Product", ImVec2(160, 35)))
    {
        productLoaded = false;
        updateSuccess = false;
        updateFailed = false;

        std::vector<Product> results = searchProducts(inputSearch);

        if (!results.empty())
        {
            loadedProduct = results[0];
            productLoaded = true;

            strcpy(updatedName, loadedProduct.name.c_str());
            updatedQuantity = loadedProduct.quantity;
            updatedPrice = (float)loadedProduct.price;
        }
        else
        {
            loadedProduct = {0, "", 0, 0.0};
            productLoaded = false;
        }
    }

    ImGui::PopStyleColor(3); // Restore button color

    ImGui::Spacing();

    if (!productLoaded && strlen(inputSearch) > 0)
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "❌ No product found with ID or Name '%s'.", inputSearch);
    }

    if (productLoaded)
    {
        ImGui::PushItemWidth(-1);
        ImGui::InputText("Updated Name", updatedName, IM_ARRAYSIZE(updatedName));
        ImGui::InputInt("Updated Quantity", &updatedQuantity);
        ImGui::InputFloat("Updated Price", &updatedPrice);
        ImGui::PopItemWidth();

        ImGui::Spacing();

        // Styled green Update button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.85f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

        if (ImGui::Button("💾 Update Product", ImVec2(180, 40)))
        {
            Product updated = {loadedProduct.id, updatedName, updatedQuantity, (double)updatedPrice};
            if (updateProduct(updated))
            {
                updateSuccess = true;
                updateFailed = false;
                loadedProduct = updated;
                strcpy(inputSearch, updatedName);
            }
            else
            {
                updateSuccess = false;
                updateFailed = true;
            }
        }

        ImGui::PopStyleColor(3); // Restore

        if (updateSuccess)
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "✅ Product updated successfully!");
        else if (updateFailed)
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "❌ Update failed. Please try again.");
    }

    ImGui::EndGroup();
}

void renderDeleteProduct()
{
    static char inputSearch[128] = "";
    static Product productToDelete = {0, "", 0, 0.0};
    static bool showConfirmDialog = false;
    static bool deleteSuccess = false;
    static bool deleteFailed = false;
    static bool productLoaded = false;

    ImGui::BeginGroup();

    ImGui::Text("🗑️ Delete Product");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Enter Product ID or Name:");
    ImGui::PushItemWidth(-1); // full-width input
    ImGui::InputText("##DeleteInput", inputSearch, IM_ARRAYSIZE(inputSearch));
    ImGui::PopItemWidth();

    productLoaded = false;

    if (strlen(inputSearch) > 0)
    {
        std::vector<Product> results = searchProducts(inputSearch);
        if (!results.empty())
        {
            productToDelete = results[0];
            productLoaded = true;
        }
    }

    ImGui::Spacing();

    if (!productLoaded && strlen(inputSearch) > 0)
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "⚠️ No product found with ID or Name '%s'", inputSearch);
    }
    else if (productLoaded)
    {
        ImGui::Text("Product to delete:");
        ImGui::BulletText("ID: %d", productToDelete.id);
        ImGui::BulletText("Name: %s", productToDelete.name.c_str());
        ImGui::BulletText("Quantity: %d", productToDelete.quantity);
        ImGui::BulletText("Price: %.2f", productToDelete.price);
    }

    ImGui::Spacing();

    bool canDelete = productLoaded;

    // 🔴 Styled delete button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.2f, 0.2f, 1.0f));

    if (ImGui::Button("🗑️ Delete Product", ImVec2(180, 40)) && canDelete)
    {
        showConfirmDialog = true;
        deleteSuccess = false;
        deleteFailed = false;
    }

    ImGui::PopStyleColor(3);

    if (showConfirmDialog)
    {
        ImGui::OpenPopup("Confirm Deletion");
    }

    // 🔒 Confirmation Popup
    if (ImGui::BeginPopupModal("Confirm Deletion", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to delete this product?\n\n");
        ImGui::BulletText("ID: %d", productToDelete.id);
        ImGui::BulletText("Name: %s", productToDelete.name.c_str());
        ImGui::BulletText("Quantity: %d", productToDelete.quantity);
        ImGui::BulletText("Price: %.2f", productToDelete.price);

        ImGui::Separator();

        // 🔴 Confirm Button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.4f, 0.4f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.2f, 0.2f, 1.0f));

        if (ImGui::Button("Yes, Delete", ImVec2(140, 35)))
        {
            if (deleteProduct(productToDelete.id))
            {
                deleteSuccess = true;
                deleteFailed = false;
                memset(inputSearch, 0, sizeof(inputSearch));
                productLoaded = false;
                productToDelete = {0, "", 0, 0.0};
            }
            else
            {
                deleteSuccess = false;
                deleteFailed = true;
            }
            showConfirmDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        // 🟤 Cancel Button (gray)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

        if (ImGui::Button("Cancel", ImVec2(140, 35)))
        {
            showConfirmDialog = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }

    ImGui::Spacing();

    if (deleteSuccess)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "✅ Product deleted successfully!");
    }
    else if (deleteFailed)
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "❌ Deletion failed. Please try again.");
    }

    ImGui::EndGroup();
}
